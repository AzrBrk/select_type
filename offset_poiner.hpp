#include<type_traits>

//提供内存对齐的位移计算，无法支持引用的成员，C++不允许创建引用的指针
namespace offset_pointer{
    //编译时计算
    //offset_ptr模拟成员在结构体中的分布， advance：跨越一个指定的size， forward_offset:寻找下一个成员的首地址
    template<std::size_t pack_size, std::size_t layer_offset, std::size_t offset> struct align_offset_ptr
    {
        static_assert(pack_size <= 8, "An alignment size exceeds 8 is not tested!");
        static constexpr std::size_t layer = layer_offset;//alignment layer as base
        static constexpr std::size_t value = offset;//当前层偏移指针
        static constexpr std::size_t half_pack = pack_size / 2;//层的半对齐
        //工具：移动到下一内存对齐的层， 层偏移指针置零
        template<std::size_t N> using advance_layer = align_offset_ptr<pack_size, layer + N, 0>;
        //当forward offset + 当前层偏移超出或等于内存对齐的值
        //假如forward offset+value==pack_size， 或则当前层偏移为0时，成员会存在当前层， 移动 size/pack_size 层，
        //否则当size%pack_size == 0, size/pack_size + 1层，因为它会被放在下一层
        //否则当size%pack_size != 0, size/pack_size + 2层(暂时没见过这种情况，数组可能会出现，但是这个类不让数组推导)
        //因为不会出现一个成员的一部分和另外一个成员在同一层的情况
        template<bool offset_overflow_pack, std::size_t forward_offset> struct advance_impl
        {
            static constexpr std::size_t forward_base = forward_offset / pack_size;
            static constexpr bool has_rest = static_cast<bool>(forward_offset % pack_size);
            static constexpr std::size_t forward_base_fixed = has_rest ? forward_base + 1 : forward_base;
            using type = advance_layer<(((forward_offset + value) == 8) ? 0 : (value == 0 ? 0 : 1)) + forward_base_fixed>;
        };
        //当forward offset + 当前层偏移小于内存对齐的值，位移不超过当前层
        //但是当forward offset超过半对齐，成员就会存储在后一半层， 否则直接移动forward offset
        template<std::size_t forward_offset> struct advance_impl<false, forward_offset>
        {
            using type = align_offset_ptr<pack_size, layer, 
                (((value + forward_offset) > half_pack) && (forward_offset >= half_pack)) ? 
                pack_size :
                value + forward_offset>;
        };
        //工具：使指针越过一个成员的位置
        template<std::size_t forward_offset> using advance = typename advance_impl<((value + forward_offset) >= pack_size), forward_offset>::type;
        //探索指针，用于寻找下一个成员的首地址，只在get中使用
        //枚举了给定的指针是否超过半对齐，内存对齐的情况
        template<std::size_t forward_size> struct forward_impl
        {
            using type = std::conditional_t<(forward_size >= pack_size), 
                advance_layer<(value == 0)? 0 : 1>, align_offset_ptr<pack_size, layer, 
                ((forward_size >= half_pack) ? ((value == 0) ? 0 : half_pack) : value) >>;
        };
        template<std::size_t forward_size> using seek = typename forward_impl<forward_size>::type;
        //基于当前的层偏移，使用探索指针寻找给定的类型的首地址
        template<class T, class type> static type& get(T* ptr)
        {
            static_assert(!std::is_array_v<type>, "calculating size of an array is not tested!");
            return *(reinterpret_cast<type*>(
                reinterpret_cast<unsigned char*>(ptr) +
                (
                    seek<sizeof(type)>::layer * pack_size +
                    seek<sizeof(type)>::value
                    )));
        }
    };
    //递归迭代，实现类似std::get的方法
    template<std::size_t N, class offset_pointer, class TL> struct offset_iter {};
    template<std::size_t N, class offset_pointer, template<class ...> class TL, class first, class ...rest>
    struct offset_iter<N, offset_pointer, TL<first, rest...>>
    {
        using recurse_type = offset_iter<N - 1, typename offset_pointer::template advance<sizeof(first)>, TL<rest...>>;
        using iter_type = typename recurse_type::iter_type;
        using type = typename recurse_type::type;

    };
    template<class offset_pointer, template<class...> class TL, class first, class ...rest>
    struct offset_iter<0, offset_pointer, TL<first, rest...>>
    {
        using type = first;
        using iter_type = offset_pointer;
    };
    //封装成函数
    template<std::size_t I, class CTL, class T> auto& get_member(T* x)
    {
        using types_iter = offset_iter<I, align_offset_ptr<alignof(T), 0, 0>, CTL>;

        return types_iter::iter_type::template get<T, typename types_iter::type>(x);
    }
    template<std::size_t I, class CTL, class T> void* get_pointer(T* x)
    {
        using types_iter = offset_iter<I, align_offset_ptr<alignof(T), 0, 0>, CTL>;
        using explore_offset = typename types_iter::iter_type::template seek<sizeof(typename types_iter::type)>;

        return reinterpret_cast<void*>(
            reinterpret_cast<unsigned char*>(x) + explore_offset::layer * alignof(T) + explore_offset::value
            );
    }

    //运行时计算
    template<class T>
    void runtime_align_offset_advance(std::size_t& layer, std::size_t &value, const std::size_t offset)
    {
        const std::size_t pack_size = alignof(T);
        const std::size_t half_pack = pack_size / 2;

        if (value + offset >= pack_size){
            const std::size_t advance_base = offset / pack_size;
            const bool has_rest = static_cast<bool>(offset % pack_size);
            const std::size_t advance_fix = has_rest ? advance_base + 1 : advance_base;

            layer += (((offset + value == pack_size) ? 0 : (value == 0 ? 0 : 1)) + advance_fix);
            value = 0;
        }
        else{
            value = ((value + offset > half_pack && offset >= half_pack) ? pack_size : (value + offset));
        }
    }

    template<class T> std::size_t runtime_align_offset_seek(const std::size_t pack_size, const std::size_t value, const std::size_t layer)
    {
        const std::size_t seek_offset = sizeof(T);
        std::size_t seek_layer{layer}, seek_value{};

        if (seek_offset + value >= pack_size){
            if (value != 0) {
                seek_layer += 1;
                seek_value = 0;
            }
        }else {
            seek_value = (seek_offset >= pack_size / 2) ? ((value == 0) ? 0 : (pack_size / 2)) : value;
        }
        return seek_layer * pack_size + seek_value;
    }
    template<class F, class TT> struct init_and_call
    {
        F f;
        TT* x;
        inline static std::size_t value = 0;
        inline static std::size_t layer = 0;

        template<class T>
        operator T() {
            unsigned char* base_ptr = reinterpret_cast<unsigned char*>(x);
            const std::size_t offset = runtime_align_offset_seek<T>(alignof(TT), value, layer);

            f(*(reinterpret_cast<T*>(base_ptr + offset)));

            runtime_align_offset_advance<TT>(layer, value, sizeof(T));

            return T{};
        };
    };

    template<class F, class T, class first, class ...rest> constexpr void for_each_member_impl(F f, T* x)
    {
        if constexpr (!requires{ T{ first{}, rest{}... }; }) {
            T{ rest{f, x}... };
        }
        else {
            for_each_member_impl<F, T, init_and_call<F, T>, first, rest...>(f, x);
        }
    }

    template<class F, class T> constexpr void for_each_member(F f, T& x)
    {
        for_each_member_impl<F, T, init_and_call<F, T>>(f, &x);
    }

}

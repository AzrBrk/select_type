#include<type_traits>

//提供内存对齐的位移计算，无法支持引用的成员，C++不允许创建引用的指针
namespace offset_pointer{
    template<typename structT, typename T>
    T& read_from_align_offset(structT* base_ptr, const std::size_t offset)
    {
        return *(reinterpret_cast<T*>(reinterpret_cast<unsigned char*>(base_ptr) + offset));
    }
    //编译时计算
    //offset_ptr模拟成员在结构体中的分布， advance：跨越一个指定的size， forward_offset:寻找下一个成员的首地址
    template<std::size_t pack_size, std::size_t layer_offset, std::size_t offset> struct align_offset_ptr
    {
        static_assert(pack_size <= 8, "An alignment size exceeds 8 is not tested!");
        static constexpr std::size_t layer = layer_offset;//alignment layer as base
        static constexpr std::size_t value = offset;//当前层偏移指针
        template<std::size_t lay> struct advance_layer
        {
            using type = align_offset_ptr<pack_size, layer + lay, 0>;
        };
        template<bool can_be_placed, std::size_t seek_offset> struct seek_impl
        {
            using type = align_offset_ptr<pack_size, layer, value>;
        };
        template<std::size_t seek_offset> struct seek_impl<false, seek_offset>
        {
            using type = typename std::conditional_t<
                (value + seek_offset > pack_size),
                typename advance_layer<1>::type,
                align_offset_ptr<pack_size, layer_offset, value + 1>
                >::template seek<seek_offset>;
        };
        template<std::size_t seek_offset> using seek = typename seek_impl<value % seek_offset == 0,seek_offset>::type;

        template<std::size_t advance_offset> struct advance_impl
        {
            using type = std::conditional_t<(value + advance_offset < pack_size),
                align_offset_ptr<pack_size, layer, value + advance_offset>,
                std::conditional_t<
                static_cast<bool>(advance_offset% pack_size),
                typename advance_layer<advance_offset / pack_size + 1>::type,
                typename advance_layer<advance_offset / pack_size>::type>
            >;
        };
        template<std::size_t advance_offset> using advance = typename advance_impl<advance_offset>::type;
        template<class structT, class T> 
        static T& get(structT* base_ptr)
        {
            using seek_ptr = seek<sizeof(T)>;
            return read_from_align_offset<structT, T>(base_ptr, seek_ptr::layer * pack_size + seek_ptr::value);
        }
    };
    //递归迭代，实现类似std::get的方法
    template<std::size_t N, class offset_pointer, class TL> struct offset_iter {};
    template<std::size_t N, class offset_pointer, template<class ...> class TL, class first, class ...rest>
    struct offset_iter<N, offset_pointer, TL<first, rest...>>
    {
        using recurse_type = offset_iter<N - 1, 
            typename offset_pointer
            ::template seek<sizeof(first)>
            ::template advance<sizeof(first)>, TL<rest...>>;
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
    template<typename structT, typename T>
    std::size_t align_offset_seek(std::size_t& layer, std::size_t& offset)
    {
        constexpr std::size_t pack_size = alignof(structT);
        constexpr std::size_t offset_size = sizeof(T);
        if (offset % offset_size) {
            offset += 1;
            if (offset + offset_size > pack_size) {
                layer += 1;
                offset = 0;
            }
            return align_offset_seek<structT, T>(layer, offset);
        }

        return pack_size * layer + offset;
    }

    template<typename structT, typename T>
    void align_offset_advance(std::size_t& layer, std::size_t& offset)
    {
        constexpr std::size_t pack_size = alignof(structT);
        constexpr std::size_t advance_size = sizeof(T);
        if (offset + advance_size < pack_size) {
            offset += advance_size;
        }
        else {
            layer += advance_size / pack_size;
            layer += advance_size % pack_size ? 1 : 0;
            offset = 0;
        }
    }

    
    template<class F, class TT> struct init_and_call
    {
        F f;
        TT* x;
        inline static std::size_t value = 0;
        inline static std::size_t layer = 0;

        template<class T>
        operator T() {
            const std::size_t off = align_offset_seek<TT, T>(layer, value);
            f(read_from_align_offset<TT, T>(x, off));
            align_offset_advance<TT, T>(layer, value);
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

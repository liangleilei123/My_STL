//
// Created by 梁磊磊 on 2023/5/5.
//

#ifndef IMPLTMENT_STL_MEMORY_H
#define IMPLTMENT_STL_MEMORY_H

namespace mystl {
    template<class Tp>
    constexpr Tp *address_of(Tp &value) noexcept {
        return &value;
    }
}

#endif //IMPLTMENT_STL_MEMORY_H

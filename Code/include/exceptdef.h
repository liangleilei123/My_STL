//
// Created by 梁磊磊 on 2023/5/3.
//

#ifndef IMPLTMENT_STL_EXCEPTDEF_H
#define IMPLTMENT_STL_EXCEPTDEF_H

#include <stdexcept>
#include <cassert>

namespace mystl {
#define MYSTL_DEBUG(expr)\
    assert(expr)        //如果表达式为假，触发断言，终止程序的执行

#define THROW_LENGTH_ERROR_IF(expr, what) \
    if((expr)) throw std::length_error(what)
}


#endif //IMPLTMENT_STL_EXCEPTDEF_H

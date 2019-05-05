#ifndef MYTYPEDEFINE_H
#define MYTYPEDEFINE_H

//定义全局的别名

#include <string>
#include <tr1/memory>
typedef std::tr1::shared_ptr<std::string> shared_string_ptr;

#endif // MYTYPEDEFINE_H

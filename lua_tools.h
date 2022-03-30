#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/*
Создать метатаблицу, связать ее поле __index с ней, записать методы из массива
methods. Массив должен заканчиваться парой значений NULL в структурах.
*/
extern void register_methods(lua_State *lua, const char *mtname, const luaL_Reg *methods);
extern const char *stack_dump(lua_State *lua);

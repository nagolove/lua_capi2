/*
Работа с вложенными таблицами
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <wchar.h>

#include "lua_tools.h"

const char *code = 
"pcall(function() foo(1) end)"
"pcall(function() foo({ 1, xxx = { 1, 1}, }) end)"
"pcall(function() foo({ 1, pos = { 1, 1}, }) end)"
"pcall(function() foo({ 'stroka', pos = { 0, 1}, }) end)"
"pcall(function() foo({ k = 'stroka', pos = { 1, 2}, }) end)"
"pcall(function() foo({ k = 'stroka', pos = { 'aa', 'b', }, }) end)"
"pcall(function() foo('ssss') end)";

int l_foo(lua_State *lua) {
    /*printf("%d\n", sizeof(wchar_t));*/

    printf("[%s]\n", stack_dump(lua));
    if (lua_isnumber(lua, 1)) {
        printf("number: %f\n", lua_tonumber(lua, 1));
    } else if (lua_istable(lua, 1)) {
        lua_pushstring(lua, "pos");
        int type = lua_gettable(lua, 1);
        printf("type %d\n", type);
        printf("lua_getable [%s]\n", stack_dump(lua));

        if (type == LUA_TTABLE) {
            int v1 = 0, v2 = 0;

            printf("[%s]\n", stack_dump(lua));
            /*printf("Напильник и карандаш\n");*/

            /*lua_pushstring(lua, "pos");*/
            lua_pushnumber(lua, 1);
            lua_gettable(lua, -2);
            v1 = lua_tonumber(lua, 1);

            printf("[%s]\n", stack_dump(lua));

            /*lua_pushstring(lua, "pos");*/
            lua_pushnumber(lua, 2);
            lua_gettable(lua, -2);
            v2 = lua_tonumber(lua, 1);

            printf("[%s]\n", stack_dump(lua));

            printf("v1 = %d, v2 = %d\n", v1, v2);
        }

    } else {
        printf("not implemented\n");
    }

    printf("--------------------------\n");

    return 0;
}

int main(void) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    // Регистрация функции с глобальным именем.
    lua_register(lua, "foo", l_foo);

    int ret = luaL_dostring(lua, code);
    if (ret != 0) {
        const char *err_msg = luaL_checkstring(lua, lua_gettop(lua));
        printf("error in lua code [%s]\n", err_msg);
    }

    lua_close(lua);

    return EXIT_SUCCESS;
}

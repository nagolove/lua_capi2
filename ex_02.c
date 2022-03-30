#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lua_tools.h"

typedef struct Object {
    int some_data;
} Object;

typedef struct Buffer {
    int some_data;
} Buffer;

static int object_tostring(lua_State *lua) {
    printf("object_tostring 01: %s\n", stack_dump(lua));
    Object *o = (Object*)lua_touserdata(lua, 1);
    char buf[32] = {0, };
    sprintf(buf, "%d", o->some_data);
    lua_pushstring(lua, buf);
    return 1;
}

static void print_stack_dump (lua_State *lua) {
    printf("%s\n", stack_dump(lua));
}

static int new_buffer(lua_State *lua) {
    // [.., ]
    printf("new buffer\n");
    Buffer *buf = (Buffer*)lua_newuserdata(lua, sizeof(Buffer));
    // [.., ud]
    buf->some_data = 11;

    printf("stack 01: %s\n", stack_dump(lua));

    // [.., ud, M]
    luaL_getmetatable(lua, "Buffer");
    // [.., ud]
    lua_setmetatable(lua, -2);

    printf("stack 02: %s\n", stack_dump(lua));

    // [.., -> ud]
    return 1;
}

static int new_object(lua_State *lua) {
    // [.., ]
    printf("new object\n");
    Object *obj = (Object*)lua_newuserdata(lua, sizeof(Object));
    // [.., ud]
    obj->some_data = 1;

    printf("stack 01: %s\n", stack_dump(lua));

    // [.., ud, M]
    luaL_getmetatable(lua, "Object");
    // [.., ud]
    lua_setmetatable(lua, -2);

    printf("stack 02: %s\n", stack_dump(lua));

    // [.., -> ud]
    return 1;
}

// Что-то делает с входящим параметром.
static int process(lua_State *lua) {
    luaL_checkudata(lua, 1, "Buffer");
    return 0;
}

static const struct luaL_Reg functions[] =
{
    {"new_object", new_object},
    {"new_buffer", new_buffer},
    {"process", process},
    {NULL, NULL}
};

static int empty(lua_State *lua) {
    printf("empty\n");
    return 0;
}

static int object_do(lua_State *lua) {
    luaL_checkudata(lua, 1, "Buffer");
    printf("does\n");
    return 0;
}

const luaL_Reg Object_methods[5] = {
    {"do1", object_do},
    {"do2", object_do},
    {"do3", object_do},
    {"__tostring", object_tostring},
    {NULL, NULL},
};

int main(void) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    /////////////////////////////////////////////
    register_methods(lua, "Buffer", Object_methods);
    /////////////////////////////////////////////

    luaL_newmetatable(lua, "Object");
    printf("stack 01: %s\n", stack_dump(lua));
    // Кладу на стек C - замыкание
    lua_pushcclosure(lua, object_tostring, 0);
    // Установка поля метатаблицы
    lua_setfield(lua, -2, "__tostring");
    printf("stack 02: %s\n", stack_dump(lua));

    // Регистрация модуля
    luaL_register(lua, "factory", functions);
    printf("stack 03: %s\n", stack_dump(lua));

    ///////////////////////////////////////
    // Создание таблички с полем-функцией и ее присванивание в глобальное
    // пространство имен.
    lua_newtable(lua);
    lua_pushstring(lua, "empty");
    lua_pushcclosure(lua, empty, 0);
    // t[k] = v where stack is [.., k, v]
    lua_settable(lua, -3);
    // pops the stack and set "system" global variable to popped value
    lua_setglobal(lua, "system");
    ///////////////////////////////////////

    // Регистрация функции с глобальным именем.
    lua_register(lua, "empty", empty);

    int ret = luaL_dofile(lua, "ex_02.lua");
    if (ret != 0) {
        const char *err_msg = luaL_checkstring(lua, lua_gettop(lua));
        printf("error in lua code [%s]\n", err_msg);
    }

    lua_close(lua);

    return EXIT_SUCCESS;
}

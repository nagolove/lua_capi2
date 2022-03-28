#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

const char *code = 
"print(\"start\")"

"local o = factory.new_object()"
"print(tostring(o))"

"local b = factory.new_buffer()"
"print(tostring(b))"

"print(\"end\")";

typedef struct Object {
    int some_data;
} Object;

typedef struct Buffer {
    int some_data;
} Buffer;

static const char *stack_dump(lua_State *lua) {
    static char ret[1024] = {0, };
    char *ptr = ret;
    int top = lua_gettop(lua);
    for (int i = 1; i <= top; i++) {
        int t = lua_type(lua, i);
        switch (t) {
            case LUA_TSTRING: 
                ptr += sprintf(ptr, "’%s’", lua_tostring(lua, i));
                break;
            case LUA_TBOOLEAN: 
                ptr += sprintf(ptr, lua_toboolean(lua, i) ? "true" : "false");
                break;
            case LUA_TNUMBER: 
                ptr += sprintf(ptr, "%g", lua_tonumber(lua, i));
                break;
            default: 
                ptr += sprintf(ptr, "%s", lua_typename(lua, t));
                break;
        }
        ptr += sprintf(ptr, " "); 
    }
    return ret;
}

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
    printf("new buffer\n");
    Buffer *buf = (Buffer*)lua_newuserdata(lua, sizeof(Buffer));
    buf->some_data = 11;

    printf("stack 01: %s\n", stack_dump(lua));

    luaL_getmetatable(lua, "Buffer");
    lua_setmetatable(lua, -2);

    printf("stack 02: %s\n", stack_dump(lua));

    return 1;
}

static int new_object(lua_State *lua) {
    printf("new object\n");
    Object *obj = (Object*)lua_newuserdata(lua, sizeof(Object));
    obj->some_data = 1;

    printf("stack 01: %s\n", stack_dump(lua));

    luaL_getmetatable(lua, "Object");
    lua_setmetatable(lua, -2);

    printf("stack 02: %s\n", stack_dump(lua));

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

int main(void) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    luaL_newmetatable(lua, "Buffer");

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

    /*
    int ret = luaL_dostring(lua, code);
    if (ret != 0) {
        printf("error in lua code\n");
    }
    */

    int ret = luaL_dofile(lua, "ex_01.lua");
    if (ret != 0) {
        printf("error in lua code\n");
    }

    lua_close(lua);

    return EXIT_SUCCESS;
}

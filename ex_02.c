#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

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
                ptr += sprintf(ptr, "ā%sā", lua_tostring(lua, i));
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

// Š§ŃŠ¾-ŃŠ¾ Š“ŠµŠ»Š°ŠµŃ Ń Š²ŃŠ¾Š“ŃŃŠøŠ¼ ŠæŠ°ŃŠ°Š¼ŠµŃŃŠ¾Š¼.
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

void register_methods(lua_State *lua, const char *mtname, const luaL_Reg *methods) {
    luaL_newmetatable(lua, mtname);
    luaL_Reg *cur = (luaL_Reg*)methods - 1;

    lua_pushvalue(lua, -1);
    lua_setfield(lua, -2, "__index");

    while (1) {
        cur++;
        if (!cur->name) {
            break;
        }
        lua_pushcclosure(lua, cur->func, 0);
        lua_setfield(lua, -2, cur->name);
    }
}

int main(void) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    /////////////////////////////////////////////
    register_methods(lua, "Buffer", Object_methods);
    /*
    luaL_newmetatable(lua, "Buffer");

    lua_pushvalue(lua, -1);
    lua_setfield(lua, -2, "__index");

    // lua_pushcclosure(lua, object_tostring, {ŠŗŠ¾Š»ŠøŃŠµŃŃŠ²Š¾ upvalue});
    lua_pushcclosure(lua, object_tostring, 0);
    // Š£ŃŃŠ°Š½Š¾Š²ŠŗŠ° ŠæŠ¾Š»Ń Š¼ŠµŃŠ°ŃŠ°Š±Š»ŠøŃŃ
    lua_setfield(lua, -2, "__tostring");

    lua_pushcclosure(lua, object_do, 0);
    lua_setfield(lua, -2, "does");
    */
    /////////////////////////////////////////////

    luaL_newmetatable(lua, "Object");
    printf("stack 01: %s\n", stack_dump(lua));
    // ŠŠ»Š°Š“Ń Š½Š° ŃŃŠµŠŗ C - Š·Š°Š¼ŃŠŗŠ°Š½ŠøŠµ
    lua_pushcclosure(lua, object_tostring, 0);
    // Š£ŃŃŠ°Š½Š¾Š²ŠŗŠ° ŠæŠ¾Š»Ń Š¼ŠµŃŠ°ŃŠ°Š±Š»ŠøŃŃ
    lua_setfield(lua, -2, "__tostring");
    printf("stack 02: %s\n", stack_dump(lua));

    // Š ŠµŠ³ŠøŃŃŃŠ°ŃŠøŃ Š¼Š¾Š“ŃŠ»Ń
    luaL_register(lua, "factory", functions);
    printf("stack 03: %s\n", stack_dump(lua));

    ///////////////////////////////////////
    // Š”Š¾Š·Š“Š°Š½ŠøŠµ ŃŠ°Š±Š»ŠøŃŠŗŠø Ń ŠæŠ¾Š»ŠµŠ¼-ŃŃŠ½ŠŗŃŠøŠµŠ¹ Šø ŠµŠµ ŠæŃŠøŃŠ²Š°Š½ŠøŠ²Š°Š½ŠøŠµ Š² Š³Š»Š¾Š±Š°Š»ŃŠ½Š¾Šµ
    // ŠæŃŠ¾ŃŃŃŠ°Š½ŃŃŠ²Š¾ ŠøŠ¼ŠµŠ½.
    lua_newtable(lua);
    lua_pushstring(lua, "empty");
    lua_pushcclosure(lua, empty, 0);
    // t[k] = v where stack is [.., k, v]
    lua_settable(lua, -3);
    // pops the stack and set "system" global variable to popped value
    lua_setglobal(lua, "system");
    ///////////////////////////////////////

    // Š ŠµŠ³ŠøŃŃŃŠ°ŃŠøŃ ŃŃŠ½ŠŗŃŠøŠø Ń Š³Š»Š¾Š±Š°Š»ŃŠ½ŃŠ¼ ŠøŠ¼ŠµŠ½ŠµŠ¼.
    lua_register(lua, "empty", empty);

    int ret = luaL_dofile(lua, "ex_02.lua");
    if (ret != 0) {
        const char *err_msg = luaL_checkstring(lua, lua_gettop(lua));
        printf("error in lua code [%s]\n", err_msg);
    }

    lua_close(lua);

    return EXIT_SUCCESS;
}

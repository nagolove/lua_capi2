#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

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

const char *stack_dump(lua_State *lua) {
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main(void) {
    lua_State *lua = luaL_newstate();
    lua_close(lua);

    return EXIT_SUCCESS;
}

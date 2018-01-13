#include <stdlib.h>
#include <znp.h>
#include "core.h"
#include "zha.h"
#include "zll.h"
#include "sm.h"
#include "aps.h"
#include "mt_af.h"
#include "mt_sys.h"
#include "mt_zdo.h"
#include "mt_util.h"
#include "ipc.h"
#include "device.h"
#include "keys.h"

/********************************
 *     Constants and macros     *
 *******************************/

#define DEMO_DEVICE_ID              0
#define GATEWAY_ADDR                0x0000

/********************************
 * Initialization state machine *
 *******************************/

static ZgSm *_init_sm = NULL;
static uint8_t _initialized = 0;
static uint8_t _reset_network = 0;

/* Callback triggered when core initialization is complete */

static void _general_init_cb(void)
{
    if(zg_sm_continue(_init_sm) != 0)
    {
        LOG_INF("Core application is initialized");
        _initialized = 1;
    }
}

static void _write_clear_flag(SyncActionCb cb)
{
    if(_reset_network)
        mt_sys_nv_write_clear_state_and_config(cb);
    else
        //mt_sys_nv_write_clear_config(cb);
        cb();
}

static void _announce_gateway(SyncActionCb cb)
{
    mt_zdo_device_annce(GATEWAY_ADDR, mt_sys_get_ext_addr(), cb);
}


static void _get_demo_device_route(SyncActionCb cb)
{
    mt_zdo_ext_route_disc_request(zg_device_get_short_addr(DEMO_DEVICE_ID), cb);
}


static ZgSmState _init_states[] = {
    {_write_clear_flag, _general_init_cb},
    {mt_sys_reset_dongle, _general_init_cb},
    {mt_sys_nv_write_nwk_key, _general_init_cb},
    {mt_sys_reset_dongle, _general_init_cb},
    {mt_sys_check_ext_addr, _general_init_cb},
    {mt_sys_nv_write_coord_flag, _general_init_cb},
    {mt_sys_nv_set_pan_id, _general_init_cb},
    {mt_sys_ping_dongle, _general_init_cb},
    {mt_util_af_subscribe_cmd, _general_init_cb},
    {zg_zll_init, _general_init_cb},
    {zg_zha_init, _general_init_cb},
    {mt_zdo_startup_from_app, _general_init_cb},
    {mt_sys_nv_write_enable_security, _general_init_cb},
    {_get_demo_device_route, _general_init_cb},
    {_announce_gateway, _general_init_cb}
};

static int _init_nb_states = sizeof(_init_states)/sizeof(ZgSmState);

/********************************
 *     Commands processing      *
 *******************************/
static void _new_device_cb(uint16_t short_addr, uint64_t ext_addr)
{
    zg_add_device(short_addr, ext_addr);
}

/********************************
 *     Commands processing      *
 *******************************/

static void _process_command_touchlink()
{
    if(_initialized)
        zg_zll_start_touchlink();
    else
        LOG_WARN("Core application has not finished initializing, cannot start touchlink");
}

static void _process_command_switch_light()
{
    uint16_t addr = 0xFFFD;
    if(_initialized)
    {
        addr = zg_device_get_short_addr(DEMO_DEVICE_ID);
        if(addr != 0xFFFD)
            zg_zha_switch_bulb_state(addr);
        else
            LOG_WARN("Device is not installed, cannot switch light");
    }
    else
    {
        LOG_WARN("Core application has not finished initializing, cannot switch bulb state");
    }

}

static void _process_user_command(IpcCommand cmd)
{
    switch(cmd)
    {
        case ZG_IPC_COMMAND_TOUCHLINK:
            _process_command_touchlink();
            break;
        case ZG_IPC_COMMAND_SWITCH_DEMO_LIGHT:
            _process_command_switch_light();
            break;
        default:
            LOG_WARN("Unsupported command");
            break;
    }
}


/********************************
 *             API              *
 *******************************/

void zg_core_init(uint8_t reset_network)
{
    LOG_INF("Initializing core application");
    _reset_network = reset_network;
    mt_af_register_callbacks();
    mt_sys_register_callbacks();
    mt_zdo_register_callbacks();
    mt_util_register_callbacks();
    if(reset_network)
        zg_keys_network_key_del();
    zg_aps_init();
    zg_ipc_register_command_cb(_process_user_command);
    zg_zha_register_new_device_joined_callback(_new_device_cb);
    zg_device_init(reset_network);

    _init_sm = zg_sm_create(_init_states, _init_nb_states);
    zg_sm_continue(_init_sm);
}

void zg_core_shutdown(void)
{
    zg_device_shutdown();
    zg_zha_shutdown();
    zg_zll_shutdown();
    zg_aps_shutdown();
}

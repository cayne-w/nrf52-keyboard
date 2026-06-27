/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "keyboard_evt.h"
#include "../ble/ble_hid_service.h"
#include "../ble/ble_services.h"
#include "action_util.h"
#include "ble_keyboard.h"
#include "host.h"
#include "keyboard.h"
#include "keyboard_matrix.h"
#include "passkey.h"
#include "power_save.h"
#include "sleep_reason.h"
#include <stdlib.h>

#include "queue.h"

NRF_SECTION_DEF(modules_init, UserEventHandler);

/**
 * 调用外部模块的事件处理
 */
static void external_event_handler(enum user_event event, void* arg)
{
    int vars_cnt = NRF_SECTION_ITEM_COUNT(modules_init, UserEventHandler);
    for (int i = 0; i < vars_cnt; i++) {
        UserEventHandler* p_var_name = NRF_SECTION_ITEM_GET(modules_init, UserEventHandler, i);
        (*p_var_name)(event, arg);
    }
}

static void internal_event_handler(enum user_event event, void* arg);

#define MAX_KBD_EVT_COUNT 20

struct event_data {
    enum user_event event;
    void* arg;
};

QUEUE(struct event_data, events, MAX_KBD_EVT_COUNT)

/**
 * 执行事件
 */
void execute_event()
{
    while (!events_empty()) {
        struct event_data* pt = events_peek();
        events_pop();
        external_event_handler(pt->event, pt->arg);
    }
}

/**
 * 触发一个事件
 * 
 * @param event: 事件类型
 * @param arg: 事件参数
 */
void trig_event(enum user_event event, void* arg)
{
    internal_event_handler(event, arg);
    if (event == USER_EVT_INTERNAL)
        return;
    if ((event == USER_EVT_STAGE && (uint32_t)arg != KBD_STATE_INITED)
        || event == USER_EVT_SLEEP)
        external_event_handler(event, arg);
    else {
        struct event_data item = {
            .arg = arg,
            .event = event,
        };
        events_push(item);
    }
}

/**
 * 触发一个事件
 * 
 * @param event: 事件类型
 * @param arg: 事件参数
 */
void trig_event_param(enum user_event event, uint8_t arg)
{
    trig_event(event, (void*)(uint32_t)arg);
}

bool power_attached;

static void internal_event_handler(enum user_event event, void* arg)
{
    uint8_t subEvent = (uint32_t)arg;
    switch (event) {
    case USER_EVT_SLEEP:
        // 休眠时，设置休眠原因便于下次免按键启动
        sleep_reason_set(subEvent == SLEEP_EVT_AUTO);
        break;
#ifdef PIN_CHARGING
    case USER_EVT_CHARGE:
#else
    case USER_EVT_USB:
#endif
        // 接入和断开电源后，禁用和启用省电模式
        power_attached = subEvent > 0;
        power_save_set_mode(!power_attached);
        ble_keyboard_powersave(!power_attached);
        break;
    case USER_EVT_BLE_STATE_CHANGE:
        if (subEvent == BLE_STATE_CONNECTED) {
            // 重连后清除矩阵历史，使 keyboard_task 重新检测所有按下的按键
            keyboard_clear();
            matrix_clear();
        } else if (subEvent == BLE_STATE_DISCONNECT) {
            // 断连时清除键盘报告状态，防止重连后 OS 认为按键仍被按住
            clear_keys();
            clear_mods();
            clear_weak_mods();
#ifndef NO_ACTION_ONESHOT
            clear_oneshot_mods();
#endif
            send_keyboard_report();
        } else if (subEvent == BLE_STATE_IDLE) {
            // 长时间没有连接，若没有接通电源则睡眠
            if (power_attached)
                advertising_slow();
            else
                sleep(SLEEP_NO_CONNECTION);
        }
        break;
    case USER_EVT_BLE_PASSKEY_STATE:
        // 需要输入Passkey，则输入passkey
        if (subEvent == PASSKEY_STATE_REQUIRE)
            passkey_req_handler();
        break;
    case USER_EVT_PROTOCOL:
        // 更改 hid 协议
        keyboard_protocol = subEvent;
        break;
    default:
        break;
    }

    // 将事件转向至HID继续处理
    hid_event_handler(event, arg);
}

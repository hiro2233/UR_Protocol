/*
    Urus Protocol - A fast, dynamic and real time universal protocol.
    Copyright (c) 2015-2017 Hiroshi Takey F. (hiro2233) <htakey@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    You can receive a copy of the GNU Lesser General Public License from
    <https://www.gnu.org/licenses/>.
 */

#pragma once

#define APM_BUILD_DIR
#ifdef APM_BUILD_DIR
#include <AP_HAL/AP_HAL.h>
#ifndef PROGMEM
#define PROGMEM
#endif
#define UPGM_UINT8(addr) addr
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#if defined(__AVR__)
#include <avr/pgmspace.h>
#define UPGM_UINT8(addr) pgm_read_byte((char *)&addr)
#else
#define PROGMEM
#define UPGM_UINT8(addr) addr
#endif // __AVR__
#endif

#if defined(__cplusplus)
#define def_urus_T template<typename T>
#else
#define def_urus_T
typedef uint8_t T;
#endif

#include "URUS_ProtocolTypes.h"

#define UR_INT_SLOT_SIZE (sizeof(URUS_Protocol::ur_internal_slot_sizes)/sizeof(urus_slot_s))
#define UR_EXT_SLOT_SIZE (sizeof(URUS_Protocol::ur_external_slot_sizes)/sizeof(urus_slot_s))

/* HARDCODED URUS HEADER ID DEFAULT */
#define URUS_HEADERID       0xCABE

/* URUS MAX SLOTS OBJECTS */
#define URUS_MAX_SLOTS      5

#define DEBUG_URUS 1

class URUS_Protocol {
public:
    URUS_Protocol();

    void configure(urus_headerid_t hdrid = {}, uint8_t ext_slot_len = 0);

    uint8_t Get_RegLen(uint8_t reg);
    urus_headerid_t Get_LocalHeaderId();
    urus_headerid_t Get_TargetHeaderId();

    def_urus_T
    uint8_t Get_DataReg(T &ptr_reg) { return _Get_DataReg((uint8_t*)&ptr_reg); };

    def_urus_T
    uint8_t Set_DataReg(T &ptr_reg) { return _Set_DataReg((uint8_t*)&ptr_reg); };

    void Get_Instance(uint8_t* ptr_reg, uint8_t instance);

    uint8_t Create_Obj(URUSObj obj_type, urus_slot_info_t& slot_info);
    uint8_t Create_Obj(uint8_t reg, urus_slot_info_t& slot_info);

    void Header16_LittlEndian(uint16_t& temp_data, uint8_t& rx_dat);
    void Header16_BigEndian(uint16_t& temp_data, uint8_t& rx_dat);
    void RingCheck_HeaderId(uint16_t& temp_data, uint8_t& rx_dat);

    uint8_t Poll_Rx_Buffer(uint8_t data);
    uint8_t Poll_Tx_Buffer(uint8_t data);

    void Prepare_Msg(uint8_t reg, uint8_t* msg_data);
    uint8_t Process_RxTxData(uint8_t rx_dat, uint8_t offset_rx);

    uint8_t Input_RxData(uint8_t data);
    uint8_t Process_Data(uint8_t data, uint8_t offset);

    uint8_t Buf_Updated(BUFType typebuf);
    uint8_t Get_Buf_Len(BUFType typebuf);
    uint8_t* Get_Buf(BUFType typebuf);

    bool CheckLittleEndianness(void);
    uint8_t* get_data1p(uint8_t instance){ return _ur_objects[instance].ptr_obj; };

    urus_slot_info_t check_slot_reg(uint8_t obj_type);

    bool checkendianess;

    uint8_t *rx_buffer;
    uint8_t *tx_buffer;
    bool is_tx_request;

    static const urus_slot_s ur_internal_slot_sizes[] PROGMEM;
    static const urus_slot_s ur_external_slot_sizes[] PROGMEM;

private:

    uint8_t _SerializeMessage(uint8_t ptr_reg, uint8_t* msg_data, uint8_t include_hdrid);
    uint8_t _Set_DataReg(uint8_t* ptr_reg);
    uint8_t _Get_DataReg(uint8_t* ptr_reg);

    urus_headerid_t _ur_hdrid_tget;
    urus_headerid_t _ur_hdrid_reply;

	urus_objects_t _ur_objects[URUS_MAX_SLOTS];
	uint8_t _count_obj;
    urus_txreg_t _ur_txreg;
    uint8_t *_rxdat;
    urus_buffer_info_t _ur_buffer_inf;
    uint8_t _in_txbuffer;
    uint8_t _in_rxbuffer;
    uint8_t _ext_slot_len;

    uint8_t _count_offset = 0;
    bool _hit = false;
    uint8_t _countlen = 0;
    uint16_t _headerid = 0;
    bool _is_tx = false;

#if (DEBUG_URUS == 1)
    char _msgbox[32];
#endif

    bool _checking_reg_len = false;
    uint8_t _counter_dat;
};

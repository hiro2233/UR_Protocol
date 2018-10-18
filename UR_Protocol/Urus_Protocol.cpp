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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "URUS_Protocol.h"

const urus_slot_s URUS_Protocol::ur_internal_slot_sizes[] PROGMEM = {
    add_reg_slot(URUS_REG_HDRID,    urus_headerid_t ),
    add_reg_slot(URUS_REG_TXREG,    urus_txreg_t    ),
    add_reg_slot(URUS_REG_RC_CHAN,  urus_rc_chan_t  ),
    add_reg_slot(URUS_REG_GPIO,     urus_gpio_t     ),
    add_reg_slot(URUS_REG_COMMAND,  urus_command_t  )
};

URUS_Protocol::URUS_Protocol():
    rx_buffer(new uint8_t),
    tx_buffer(new uint8_t),
    _count_obj(0),
    _rxdat(new uint8_t),
    _in_txbuffer(0),
    _in_rxbuffer(0),
    _counter_dat(0)
{}

void URUS_Protocol::configure(urus_headerid_t hdrid, uint8_t ext_slot_len)
{
    if (hdrid.hdrid != 0) {
        _ur_hdrid_tget.hdrid = hdrid.hdrid;
    } else {
        _ur_hdrid_tget.hdrid = URUS_HEADERID;
    }

    checkendianess = CheckLittleEndianness();
    _ext_slot_len = ext_slot_len;
}

urus_slot_info_t URUS_Protocol::check_slot_reg(uint8_t obj_type)
{
    urus_slot_info_t slot_info;

    for (uint8_t i2 = 0; i2 < _count_obj; i2++) {
        if ((_ur_objects[i2].reg == obj_type) && (_ur_objects[i2].ptr_obj != nullptr)) {
            slot_info.UError = slot_error_t::IN_USE;
            slot_info.reg = obj_type;
            slot_info.pos = i2;
            slot_info.len = Get_RegLen(slot_info.reg);
            break;
        }
    }

    return slot_info;
}

uint8_t URUS_Protocol::_SerializeMessage(uint8_t reg, uint8_t* msg_data, uint8_t include_hdrid)
{
	uint8_t len_urus_hdrid = sizeof(urus_headerid_t);
    urus_slot_info_t slot_info;
    slot_info = check_slot_reg(reg);

    if (slot_info.reg != 0) {
        if (include_hdrid) {
			memcpy(msg_data, &_ur_hdrid_tget.hdrid, len_urus_hdrid);
        } else {
            memcpy(msg_data, &_ur_hdrid_reply.hdrid, len_urus_hdrid);
        }

        memcpy(&msg_data[len_urus_hdrid], _ur_objects[slot_info.pos].ptr_obj, slot_info.len);

        return 1;
    }
    return 0;
}

uint8_t URUS_Protocol::Get_RegLen(uint8_t reg)
{
    uint8_t len = 0;

    if (reg == 0) {
        return len;
    }

    uint8_t ur_int_size = UR_INT_SLOT_SIZE;
    if ((ur_int_size > 0) && (reg >= BASE_ADDR_URUS) && (reg < URUS_REG_COUNT)) {
        len = UPGM_UINT8(ur_internal_slot_sizes[reg - BASE_ADDR_URUS].len);
        return len;
    }

    if ((_ext_slot_len > 0) && ((reg - URUS_REG_COUNT) < _ext_slot_len) && (reg >= URUS_REG_COUNT)) {
        len = UPGM_UINT8(ur_external_slot_sizes[reg - URUS_REG_COUNT].len);
    }

    return len;
}

uint8_t URUS_Protocol::_Set_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    reg = ptr_reg[0];
    urus_slot_info_t slot_info;
    slot_info = check_slot_reg(reg);

    if (slot_info.UError == slot_error_t::NONE) {
        return 0;
    }

    if (slot_info.reg != 0) {
        memcpy(_ur_objects[slot_info.pos].ptr_obj, ptr_reg, slot_info.len);
        return 1;
    }

    return 0;
}

uint8_t URUS_Protocol::_Get_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    reg = ptr_reg[0];
    urus_slot_info_t slot_info;
    slot_info = check_slot_reg(reg);

    if (slot_info.UError == slot_error_t::NONE) {
        return 0;
    }

    if (slot_info.reg != 0) {
        memcpy(ptr_reg, _ur_objects[slot_info.pos].ptr_obj, slot_info.len);
        return 1;
    }

    return 0;
}

urus_headerid_t URUS_Protocol::Get_LocalHeaderId()
{
    urus_headerid_t tmphdrid;
    tmphdrid.hdrid = URUS_HEADERID;
    return tmphdrid;
}

urus_headerid_t URUS_Protocol::Get_TargetHeaderId()
{
    return _ur_hdrid_tget;
}

void URUS_Protocol::Header16_LittlEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = temp_data >> 8;
    temp_data = (uint16_t)((((uint16_t)rx_data) << 8) | temp_data );
}

void URUS_Protocol::Header16_BigEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = (uint16_t)((temp_data << 8) | rx_data );
}

void URUS_Protocol::RingCheck_HeaderId(uint16_t& temp_data, uint8_t& rx_data)
{
    if (checkendianess) {
        Header16_LittlEndian(temp_data, rx_data);
    } else {
		Header16_BigEndian(temp_data, rx_data);
	}
}

bool URUS_Protocol::CheckLittleEndianness()
{
    uint16_t a=0xABCD;
    if (*((uint8_t *)&a)==0xAB) {
	    return false;
	} else {
		return true;
    }

}

/// <summary>
/// Prepare messages and serialize data dest.
/// Parameters.-
/// @ptr_reg   : Source data to serialize.
/// @msg_data: Destination data serialized to send.
/// </summary>
void URUS_Protocol::Prepare_Msg(uint8_t reg, uint8_t* msg_data)
{
	_SerializeMessage(reg, msg_data, 1);
}

uint8_t URUS_Protocol::Buf_Updated(BUFType typebuf)
{
    switch (typebuf) {
    case URUS_TX_BUF:
    {
        return _ur_buffer_inf.txupdated;
    }

    case URUS_RX_BUF:
    {
        return _ur_buffer_inf.rxupdated;
    }

    default:
        return 0;
    }

    return 0;
}

uint8_t URUS_Protocol::Get_Buf_Len(BUFType typebuf)
{
    switch (typebuf) {
        case URUS_TX_BUF:
        {
            return _ur_buffer_inf.txlen;
        }

        case URUS_RX_BUF:
        {
            return _ur_buffer_inf.rxlen;
        }

        default:
            return 0;
    }

    return 0;
}

uint8_t* URUS_Protocol::Get_Buf(BUFType typebuf)
{

    uint8_t *buffer;

    switch (typebuf) {
        case URUS_TX_BUF:
        {
            _ur_buffer_inf.txupdated = 0;
            buffer = tx_buffer;
			return buffer;
        }

        case URUS_RX_BUF:
        {
            _ur_buffer_inf.rxupdated = 0;
            buffer = rx_buffer;
            return buffer;
        }

        default:
            break;
    }

    return nullptr;
}

uint8_t URUS_Protocol::Create_Obj(URUSObj obj_type, urus_slot_info_t& slot_info)
{

    slot_info = check_slot_reg((uint8_t)obj_type);

    if (slot_info.UError == slot_error_t::IN_USE) {
        return 0;
    }

    slot_info.reg = obj_type;
    uint8_t lentype;
    lentype = Get_RegLen(slot_info.reg);

    if (lentype != 0) {
        _ur_objects[_count_obj].reg = obj_type;
        _ur_objects[_count_obj].ptr_obj = new uint8_t[lentype];
        memcpy(_ur_objects[_count_obj].ptr_obj, &slot_info.reg, sizeof(slot_info.reg));
        _ur_objects[_count_obj].pos = _count_obj;
        _ur_objects[_count_obj].len = lentype;
        _count_obj++;
        slot_info.UError = slot_error_t::CREATED;
        return 1;
    }

    return 0;
}

uint8_t URUS_Protocol::Create_Obj(uint8_t reg, urus_slot_info_t& slot_info)
{
    return Create_Obj((URUSObj)reg, slot_info);
}

void URUS_Protocol::Get_Instance(uint8_t *ptr_reg, uint8_t instance)
{
    uint8_t reg;
    reg = ptr_reg[0];
    urus_slot_info_t slot_info;
    slot_info = check_slot_reg(reg);

    if (slot_info.UError == slot_error_t::NONE) {
        return;
    }

    if (slot_info.reg != 0) {
        uint8_t lentype;
        lentype = Get_RegLen(slot_info.reg);
        memcpy(ptr_reg, _ur_objects[slot_info.reg].ptr_obj, lentype);
        return;
    }
}

uint8_t URUS_Protocol::Process_Data(uint8_t data, uint8_t offset)
{

    if (_in_txbuffer ) {
        return 0;
    }

    if (_in_rxbuffer) {
        return 0;
    }

    if (_count_offset < offset) {
        _count_offset++;
        return 0;
    }

    //Check if hit with headerid
    if (!_hit) {
        /*  We check constantly the headerid and compare if it's
            got again and check if the last data received was in process,
            if it's trigger, thats very bad and means we have a fragmented
            data, maybe master have timming problems or something else.
        */
        RingCheck_HeaderId(_headerid, data);
    }

    /* It's jumping when a new data register are incoming,
       when hit with incomming header id, then enter to complete register id verification,
       first we check if the register id is a command request or are sending a data
       payload, if got a command request then jump to send the register requested,
       if we doesn't got a command request then we enter to buffering process,
       then we store payload length as fast as possible into URUS Objects slots whenever
       if there is one space available, if not, then we throw an error info.
    */
    if (_headerid == (uint16_t)URUS_HEADERID) {
            if (!_hit) {
                _hit = true;
                _checking_reg_len = true;
                return 0;
            }

            if (_checking_reg_len) {
                _countlen = Get_RegLen(data);
                if (_countlen == 0) {
#if (DEBUG_URUS == 1)
                    sprintf(_msgbox, "counter ");
#endif
                    goto RESET_ALL;
                }
            }

            if ((data == _ur_txreg.reg) && (_is_tx == false) && (_countlen <= sizeof(urus_txreg_t))) {
                _is_tx = true;
            }

            if (_is_tx) {
                _checking_reg_len = false;
                delete tx_buffer;
                tx_buffer = new uint8_t[_countlen];
                _ur_buffer_inf.txlen = _countlen;
                tx_buffer[0] = data;
                _in_txbuffer = 1;
#if (DEBUG_URUS == 1)
                sprintf(_msgbox, "Is_tx " );
#endif
                goto RESET_ALL;
            }

            _checking_reg_len = false;
            delete rx_buffer;
            rx_buffer = new uint8_t[_countlen];
            _ur_buffer_inf.rxlen = _countlen;
            rx_buffer[0] = data;
            _in_rxbuffer = 1;

        if (_hit) {
#if (DEBUG_URUS == 1)
            sprintf(_msgbox, "recibido hit " );
#endif
            goto RESET_ALL;

        }
    }

    return 0;

RESET_ALL:
    _counter_dat = 0;
    _hit = false;
    _headerid = 0;
    _countlen = 0;
    _count_offset = 0;
    _is_tx = false;
	_checking_reg_len = false;
#if (DEBUG_URUS == 1)
	printf("all reset %s! \n", _msgbox);
#endif
    return 1;
}

uint8_t URUS_Protocol::Input_RxData(uint8_t data)
{

    if ((!_in_txbuffer) && (!_in_rxbuffer)) {
        Process_Data(data, 0);
    }

    if (_in_rxbuffer) {
        Poll_Rx_Buffer(data);
    }

    if (_in_txbuffer) {
        Poll_Tx_Buffer(data);
    }

    return 0;
}

uint8_t URUS_Protocol::Poll_Rx_Buffer(uint8_t data)
{

    if (_in_rxbuffer) {

		if (_counter_dat < (_ur_buffer_inf.rxlen)) {
            rx_buffer[_counter_dat] = data;

        }

        if (_counter_dat >= (_ur_buffer_inf.rxlen - 1)) {

            urus_slot_info_t slotinfo;
            Create_Obj(rx_buffer[0], slotinfo);

            _ur_buffer_inf.rxupdated = _Set_DataReg(&rx_buffer[0]);
            uint8_t reglen = Get_RegLen(rx_buffer[0]);

            for (uint8_t i = 0; i < reglen; i++) {
                rx_buffer[i] = 0;
            }

            _in_rxbuffer = 0;
        }
        _counter_dat++;
    }

    return 0;
}

uint8_t URUS_Protocol::Poll_Tx_Buffer(uint8_t data)
{

    if (_ur_buffer_inf.txupdated) {
        _in_txbuffer = 0;
        _counter_dat = 0;
        return 0;
    }

    if (_in_txbuffer) {

		if (_counter_dat < _ur_buffer_inf.txlen) {
            tx_buffer[_counter_dat] = data;

        }

        if (_counter_dat >= (_ur_buffer_inf.txlen - 1)) {
            urus_slot_info_t slotinfo;
            Create_Obj(tx_buffer[0], slotinfo);

            _Set_DataReg(&tx_buffer[0]);

            urus_txreg_t urtxreg;
            _Get_DataReg((uint8_t*)&urtxreg);

            delete tx_buffer;
            uint8_t cntlen = Get_RegLen(urtxreg.TargetReg);

            _ur_buffer_inf.txlen = cntlen;
            tx_buffer = new uint8_t[cntlen];

            _ur_hdrid_reply.hdrid = urtxreg.RplyHdrid;
            _ur_buffer_inf.txupdated = _SerializeMessage(urtxreg.TargetReg, tx_buffer, 0);

            _in_txbuffer = 0;
        }
        _counter_dat++;
    }

    return 0;
}

// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifdef APM_BUILD_DIRECTORY
#include <AP_HAL/AP_HAL.h>
const extern AP_HAL::HAL& hal;
#else
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#include "Urus_Protocol.h"

#define DEBUG_URUS 1

urus_objects_t CL_UrusProtocol::_ur_objects[URUS_MAX_SLOTS];
//uint8_t *CL_UrusProtocol::rxdat;
uint8_t CL_UrusProtocol::_count_obj = 0;

CL_UrusProtocol::CL_UrusProtocol(uint16_t hdrid):
    //_count_obj(0),
    //_ur_objects(new urus_objects_t[URUS_MAX_SLOTS]),
    _rxdat(new uint8_t),
    //rxdattemp(new uint8_t[10]),
    rx_buffer(new uint8_t[32]),
    tx_buffer(new uint8_t),
    hit_header(false),
    counter_dat(0),
    count_len(0),
    fire_data_rx(false),
    is_tx_request(false),
    fire_data_tx(false)
{
    if (hdrid != 0) {
        _ur_headerid.hdrid = hdrid;
    } else {
        _ur_headerid.hdrid = URUS_HEADERID;
    }
}

CL_UrusProtocol::CL_UrusProtocol(void):
    //_count_obj(0),
    //_ur_objects(new urus_objects_t[URUS_MAX_SLOTS]),
    _rxdat(new uint8_t),
    //rxdattemp(new uint8_t[10]),
    rx_buffer(new uint8_t[32]),
    hit_header(false),
    counter_dat(0),
    count_len(0),
    fire_data_rx(false),
    is_tx_request(false),
    fire_data_tx(false)
{
    _ur_headerid.hdrid = URUS_HEADERID;
}

uint8_t CL_UrusProtocol::_SerializeMessage(uint8_t reg, uint8_t* msg_data, uint8_t include_hdrid)
{
    uint8_t len_urus_hdrid = sizeof(urus_headerid_t);

    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            uint8_t len_rc_chan = sizeof(urus_rc_chan_t);

            if (include_hdrid) {
                memcpy(msg_data, &_ur_headerid, len_urus_hdrid);
            }

            memcpy(&msg_data[(len_urus_hdrid * include_hdrid)], _ur_objects[0].ptr_obj, len_rc_chan);

            return 1;
      }
      default:
            return 1;
    }
    return 0;
}

uint8_t CL_UrusProtocol::Check_RegLen(uint8_t reg)
{
    uint8_t len;
    switch (reg) {

        case URUS_REG_TXREG:
        {
            len = sizeof(urus_txreg_t);
            return len;
        }

        case URUS_REG_RC_CHAN:
        {
            len = sizeof(urus_rc_chan_t);
            return len;
        }

        default:
            len = 0;
    }
    return len;
}

void CL_UrusProtocol::_Set_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    //memcpy(&reg, ptr_reg, 1);
    reg = ptr_reg[0];

    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(_ur_objects[0].ptr_obj, ptr_reg, sizeof(urus_rc_chan_t));
            //hal.console->printf("CH1_set: %u\n", ur->ch1);
            break;
        }
        default:
            break;
    }
}

void CL_UrusProtocol::_Get_DataReg(uint8_t* ptr_reg)
{
    uint8_t reg;
    //memcpy(&reg, ptr_reg, 1);
    reg = ptr_reg[0];
    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(ptr_reg, _ur_objects[0].ptr_obj, sizeof(urus_rc_chan_t));
            //hal.console->printf("CH1: %u\n", ur->ch1);
            break;
        }
        default:
            break;
    }
}

urus_headerid_t CL_UrusProtocol::Get_HeaderId(void)
{
    return _ur_headerid;
}

void CL_UrusProtocol::Header16_LittlEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = temp_data >> 8;
    temp_data = ((((uint16_t)rx_data) << 8) | temp_data );
}

void CL_UrusProtocol::Header16_BigEndian(uint16_t& temp_data, uint8_t& rx_data)
{
    temp_data = (uint16_t)((temp_data << 8) | rx_data );
}

void CL_UrusProtocol::RingCheck_HeaderId(uint16_t& temp_data, uint8_t& rx_data)
{
    if (checkendianess) {
        Header16_LittlEndian(temp_data, rx_data);
    } else {
        Header16_BigEndian(temp_data, rx_data);
    }

}

bool CL_UrusProtocol::CheckLittleEndianness() 
{
    uint16_t a=0x1234;
    if (*((uint8_t *)&a)==0x12) {
	    return false;
        //hal.console->printf("BIG\n");
	} else {
        return true;
        //hal.console->printf("LITTLE\n");
    }
}

/// <summary>
/// Prepare messages and serialize data dest.
/// Parameters.-
/// @ptr_reg   : Source data to serialize.
/// @msg_data: Destination data serialized to send.
/// </summary>
void CL_UrusProtocol::Prepare_Msg(uint8_t reg, uint8_t* msg_data)
{
    _SerializeMessage(reg, msg_data, 1);
}

void CL_UrusProtocol::Create_Obj(URUSObj obj_type, urus_slot_info_t& slot_info)
{
    slot_error_t uerror;
    slot_info.reg = obj_type;
    hal.console->printf("Slot obj_type: 0x%x\n", slot_info.reg);

    for (uint8_t i=0; i<10; i++) {
        if (_ur_objects[_count_obj].ptr_obj == NULL) {
            slot_info.UError = uerror.CREATED;
        }
    }

    switch (obj_type) {
        case URUS_REG_RC_CHAN:
        {
            _ur_objects[_count_obj].reg = obj_type;
            _ur_objects[_count_obj].ptr_obj = new uint8_t[sizeof(urus_rc_chan_t)];
            _ur_objects[_count_obj].Len = sizeof(urus_rc_chan_t);
            _count_obj++;
            break;
        }
        default:
            break;
    }

    for (uint8_t i = 0; i < _count_obj; i++) {
        hal.console->printf("OBJECT: %x Count: %u\n", _ur_objects[i].reg, _count_obj);
    }
}

uint8_t CL_UrusProtocol::Buf_Updated(BUFType typebuf)
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

uint8_t CL_UrusProtocol::Get_Buf_Len(BUFType typebuf)
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

const uint8_t* CL_UrusProtocol::Get_Buf(BUFType typebuf)
{
    _ur_buffer_inf.txupdated = 0;

    uint8_t *buffer;

    switch (typebuf) {
        case URUS_TX_BUF:
        {
            buffer = tx_buffer;
            return tx_buffer;
        }

        case URUS_RX_BUF:
        {
            buffer = rx_buffer;
            return rx_buffer;
        }

        default:
            return NULL;
    }

    return buffer;
}

poll_rx_tx_info_t CL_UrusProtocol::Poll_Tx_CommandRequest()
{
    //poll_rx_tx_error_t poll_error;
    poll_rx_tx_info_t poll_inf;

    if (fire_data_tx) {

        fire_data_tx = false;
        for (uint8_t i = 0; i < count_len; i++) {
            Process_RxTxData(rx_buffer[i], 0);
            rx_buffer[i] = 0;
        }

        uint8_t len;
        len = Check_RegLen(_ur_txreg.TargetReg);
        delete tx_buffer;
        tx_buffer = new uint8_t[len];
        _ur_buffer_inf.txupdated = _SerializeMessage(_ur_txreg.TargetReg, tx_buffer, 0);

        if (_ur_buffer_inf.txupdated) {
            _ur_buffer_inf.txlen = len;

            poll_inf.poll_error = poll_rx_tx_error_t::PO_TX_OK;
        }

        is_tx_request = false;
        fire_data_rx = false;
        counter_dat = 0;
    }

    return poll_inf;
}

uint8_t CL_UrusProtocol::Poll_Rx_Buffer()
{
    if (fire_data_rx) {

        fire_data_rx = false;

        for (uint8_t i = 0; i < count_len; i++) {
            Process_RxTxData(rx_buffer[i], 0);
            rx_buffer[i] = 0;
        }

        return 1;
    }

    return 0;
}

poll_rx_tx_info_t CL_UrusProtocol::Poll_RxTxData(uint8_t data)
{
    poll_rx_tx_info_t poll_inf;

    if ((!fire_data_rx) && (!is_tx_request)) {
        if (hit_header) {
            rx_buffer[counter_dat] = data;
            Process_RxTxData(data, 0);
            if (count_len == 0) {
               count_len = Check_RegLen(data);
            }
            if (((count_len - counter_dat) == 0) && (count_len != 0)) {
                if (count_len <= sizeof(urus_txreg_t) ) {
                    fire_data_tx = true;
                    is_tx_request = true;
                    fire_data_rx = false;
                    hit_header = false;
                } else {
                    fire_data_rx = true;
                    hit_header = false;
                    is_tx_request = false;
                }

                poll_inf.poll_error = poll_rx_tx_error_t::PO_RX_OK;
            }
            counter_dat++;
        } else {
            counter_dat = 0;
            Process_RxTxData(data, 0);
        }
    }
 
    poll_rx_tx_info_t poll_temp = Poll_Tx_CommandRequest();
    poll_inf.poll_error = poll_temp.poll_error | poll_inf.poll_error;

    return poll_inf;
}

void CL_UrusProtocol::Create_Obj(uint8_t reg, urus_slot_info_t& slot_info)
{
    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            Create_Obj(URUSObj::URUS_REG_RC_CHAN, slot_info);
            hal.console->printf("Slot uint8: 0x%x\n", slot_info.reg);
            break;
        }
        default:
            break;
    }
}

void CL_UrusProtocol::Get_Instance(uint8_t *ptr_reg, uint8_t instance)
{
    uint8_t reg;
    reg = ptr_reg[0];
    switch (reg) {
        case URUS_REG_RC_CHAN:
        {
            memcpy(ptr_reg, _ur_objects[instance].ptr_obj, sizeof(urus_rc_chan_t));
            break;
        }
        default:
            break;
    }
}

uint8_t CL_UrusProtocol::Process_RxTxData(uint8_t rx_dat, uint8_t offset_rx)
{
    static uint8_t counterdat = 0;
    static uint8_t count_offset = 0;
    static bool hit = false;
    static uint8_t countlen = 0;
    static uint16_t headeridcheck = 0;
    static uint16_t headerid = 0;
    static bool is_tx = false;

    if (count_offset < offset_rx) {
        count_offset++;
        return 0;
    }

    //Check if hit with headerid
    if (!hit) {
        RingCheck_HeaderId(headerid, rx_dat);
    } else {
        /*  We check constantly the headerid and compare if it's
            got again and check if the last data received was in process,
            if it's trigger, thats very bad and means we have a fragmented
            data, maybe master are sending very fast the enterely command
            or something else.
        */
        RingCheck_HeaderId(headeridcheck, rx_dat);

        if ((headerid == headeridcheck) && (hit)) {
            hal.console->printf("Fragmented\n");
            goto RESET_ALL;
        }
    }

    /* It's are jumping when a new data register are incoming,
       when hit with header id, then enter to complete register id verification,
       first we check if the register id is a command request or are sending a data
       payload, if got a command request then jump to send process the register requested,
       if we doesn't got a command request then we enter to buffering process if got it,
       then we store payload length as fast as possible in to URUS Objects slots
       if there is one space available, if not, we throw an error info.
    */

    if ((headerid == (uint16_t)URUS_HEADERID) && (!hit_header)) {

        if (hit) {
            if (counterdat != 0) {
                _rxdat[counterdat] = rx_dat;
            }
        } else {
            hit = true;
            hit_header = hit;
            return 0;
        }

        //check the size register, if got a valid address register then return the lenght.
        if ((counterdat == 0) && (hit)) {
            //hal.console->printf("REG: %u\n", dat1);
            countlen = Check_RegLen(rx_dat);
            delete _rxdat;
            _rxdat = new uint8_t[(countlen + 2)];
            _rxdat[0] = rx_dat;
            if (countlen == 0) {
                //hal.console->printf("Reset\n");
                goto RESET_ALL;
            }
        }

        if ((_rxdat[0] == _ur_txreg.reg) && (is_tx == false)) {
            is_tx = true;
            is_tx_request = is_tx;
            counterdat++;
            return 0;
        }

        if (is_tx) {
            hal.console->printf("Proc TX\n");
            _ur_txreg.TargetReg = _rxdat[1];
            is_tx = false;
            goto RESET_ALL;
        }

        counterdat++;
        if ((counterdat >= countlen) && (!is_tx))
        {
            urus_slot_info_t slotinfo;
            Create_Obj(_rxdat[0], slotinfo);
            _Set_DataReg(_rxdat);

#ifdef DEBUG_URUS
            hal.console->printf("RECIBIDO SlotError: %u \n", slotinfo.UError);
            //hal.console->printf("RECIBIDO dat0:%x dat1:%x dat2:%x dat3:%x dat4:%x dat5:%x dat6:%x SPSR:%x \n", _rxdat[0], _rxdat[1], _rxdat[2], _rxdat[3], _rxdat[4], _rxdat[5], _rxdat[6], headerid); 
#endif
            goto RESET_ALL;
        }
     }

return 0;

RESET_ALL:
    counterdat = 0;
    counter_dat = counterdat;
    hit = false;
    hit_header = hit;
    headerid = 0;
    headeridcheck = 0;
    countlen = 0;
    count_len = countlen;
    fire_data_rx = false;
    count_offset = 0;
    is_tx = false;
return 1;
}

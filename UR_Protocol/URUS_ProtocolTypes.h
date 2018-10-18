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
#pragma pack(push, 1)

/* Core define types. */

/* URUS Protocol base objects addres registers */
#define BASE_ADDR_URUS    0x0B

/* URUS Protocol base  objects names registers */
enum URUSObj {
    URUS_REG_HDRID = BASE_ADDR_URUS,
	URUS_REG_TXREG,
	URUS_REG_RC_CHAN,
	URUS_REG_GPIO,
	URUS_REG_COMMAND,
    URUS_REG_COUNT
};

enum BUFType {
    URUS_TX_BUF = 0,
	URUS_RX_BUF
};

/**************************************************************************
 *
 *                          URUS_REG_CORE_BEGIN
 *
 **************************************************************************/

typedef struct __urus_slot_s
{
    uint8_t reg;
    uint8_t len;
} urus_slot_s;

/// <summary>
/// Here, we declare all objects registers for URUS protocol that we need.
/// It's the CORE MAIN base objects registers for URUS protocol.
/// </summary>

/// This is the Header ID for each UrusProtocol Object.
/// OBJECT NAME: "urus_headerid_t".
/// @HDRId 16 Bits UrusProtocol Object Header identification.
typedef struct __urus_headerid_t
{
    uint16_t hdrid;

public:
	__urus_headerid_t():
	   hdrid(0)
	{}

} urus_headerid_t;

/// This is the CORE MAIN Register Object and where
/// the all REGISTER OBJECTS will allocate in slot and hold it.
/// OBJECT NAME: "urus_objects_t".
/// @reg URUS Address register.
/// @ptr_obj Pointer where allocate Registers Objects dinamically.
/// @len The length of data hold.
typedef struct __urus_objects_t
{
    uint8_t reg;
    uint8_t *ptr_obj;
    uint8_t pos;
    uint8_t len;

public:
	__urus_objects_t():
	   reg(0),
       ptr_obj(NULL),
       pos(0),
       len(0)
	{}

} urus_objects_t;

/// Register object status and info.
/// OBJECT NAME: "urus_slot_info_t".
/// @Reg URUS Address register.
/// @UError Handle slot error types.
/// NOTE: This one it's only for local manipulation.
/// "slot_error_t" Typedef is only for "urus_slot_info_t".
typedef struct __slot_error_t {
    enum UERROR {
        NONE = 0,
        FULL,
        CREATED,
        IN_USE
    };
} slot_error_t;

typedef struct __urus_slot_info_t
{
    uint8_t reg;
    uint8_t UError;
    uint8_t pos;
    uint8_t len;

public:
	__urus_slot_info_t():
	   reg(0),
       UError(slot_error_t::NONE),
       pos(0),
       len(0)
	{}

} urus_slot_info_t;

/// Register object request for TX.
/// OBJECT NAME: "urus_txreg_t".
/// @reg Address register.
/// @Target The target for register object request.
typedef struct __urus_txreg_t
{
    uint8_t reg;
    uint8_t TargetReg;
    uint16_t RplyHdrid;

public:
	__urus_txreg_t():
	   reg(URUS_REG_TXREG),
       TargetReg(0),
       RplyHdrid(0)
	{}

} urus_txreg_t;

typedef struct __urus_buffer_info_t
{
    uint8_t rxupdated;
    uint8_t rxlen;
    uint8_t txupdated;
    uint8_t txlen;

public:
	__urus_buffer_info_t():
        rxupdated(0),
        rxlen(0),
        txupdated(0),
        txlen(0)
	{}

} urus_buffer_info_t;

typedef struct __poll_rx_tx_error_t
{
    enum PO_ERROR {
        PO_NONE = 0,
        PO_RX_OK,
        PO_TX_OK,
        PO_RX_TX_OK
    };
} poll_rx_tx_error_t;

typedef struct __poll_rx_tx_info_t
{
    int poll_error;

public:
	__poll_rx_tx_info_t():
        poll_error(poll_rx_tx_error_t::PO_NONE)
	{}

} poll_rx_tx_info_t;

/**************************************************************************
 *
 *                          URUS_REG_CORE_END
 *
 **************************************************************************/

#include "structures/URUS_ProtocolTypesEXT.h"

#define add_reg_slot(_enuname, _typename)           \
    { _enuname, sizeof(_typename) }
#define add_reg(name, type) add_reg_slot(name, type)

#pragma pack(pop)

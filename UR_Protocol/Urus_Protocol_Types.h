/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#pragma once
#pragma pack(push, 1)

/* Core define types. */

/* URUS HEADER ID DEFAULT */
#define URUS_HEADERID       0xCABE

/* URUS MAX SLOTS OBJECTS */
#define URUS_MAX_SLOTS      5

/* URUS Protocol base objects addres registers */
#define BASE_ADDR_URUS    0x0A

/* URUS Protocol base  objects names registers */
enum URUSObj {
    URUS_REG_TXREG = BASE_ADDR_URUS,
	URUS_REG_RC_CHAN,
    URUS_REG_COUNT
};

enum BUFType {
    URUS_TX_BUF = 0,
    URUS_RX_BUF
};

/// <summary>
/// Here, we declare all objects registers for URUS protocol that we need.
/// It's the CORE MAIN base objects registers for URUS protocol.
/// </summary>

/// This is the Header ID for each UrusProtocol Object.
/// OBJECT NAME: "urus_headerid_t".
/// @HDRId 16 Bits UrusProtocol Object Header identification.

//////////////////////////////// URUS_REG_CORE_BEGIN
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
       pos(0),
       ptr_obj(NULL),
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
      static const uint8_t NONE = 0;
      static const uint8_t FULL = 1;
      static const uint8_t CREATED = 2;
      static const uint8_t IN_USE = 3;
} slot_error_t;

typedef struct __urus_slot_info_t
{
    uint8_t reg;
    uint8_t UError;

public:
	__urus_slot_info_t():
	   reg(0),
       UError(slot_error_t::NONE)
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

public:
	__urus_txreg_t():
	   reg(URUS_REG_TXREG),
       TargetReg(0)
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
    static const uint8_t PO_NONE = 0;
    static const uint8_t PO_RX_OK = 1;
    static const uint8_t PO_TX_OK = 2;
    static const uint8_t PO_RX_TX_OK = 3;
} poll_rx_tx_error_t;

typedef struct __poll_rx_tx_info_t
{
    int poll_error;

public:
	__poll_rx_tx_info_t():
        poll_error(poll_rx_tx_error_t::PO_NONE)
	{}

} poll_rx_tx_info_t;

//////////////////////////////// URUS_REG_CORE_END

/// <summary>
/// Here we declare all GENERAL registers objects that we need.
/// You can add N registers objects, please be becarefull on adding.
/// The only restriction is the RAM limit of the target board
/// and you can slot in up to 255.
/// </summary>

/// Register object for RC_CHANNELS
/// OBJECT NAME: "urus_rc_chan_t".
/// @reg Address register.
/// @CH1 Field to
/// @CH2 are RC_CHANNEL.

typedef struct __urus_rc_chan_t
{
    uint8_t reg;
    uint16_t CH1;
    uint16_t CH2;

public:
	__urus_rc_chan_t():
        reg(URUS_REG_RC_CHAN),
        CH1(0),
        CH2(0)
	{}

} urus_rc_chan_t;

#pragma pack(pop)
// license:BSD-3-Clause
// copyright-holders:Robbbert
/***************************************************************************************************

PINBALL
Spinball (previously Inder)

Hardware is much the same as Inder, except the digital display is replaced by a DMD controlled by
 a i8031.

Each machine has 2 identical sound card, each with different roms. One card plays music and the
 other does effects. The first version of the cards uses MSM5205, this was upgraded to MSM6585
 (Jolly Park, Verne's World).

There are mistakes in the sound board schematic: IC14 pin12 goes to IC5 pin13 only. IC16 pin 22
 is the CS0 line only. So, they are not joined but are separate tracks. Also, according to PinMAME,
 the outputs of IC11 are all wrong. They should be (from top to bottom): A16, A17, A18, NC, NC,
 CS2, CS1, CS0.

Also, very unobvious is the fact that PIA ports A and B are swapped around compared to the Inder
 soundcard.

Status:
- Metal Man is the only playable machine.
- The other machines firstly need balls in the machine to get attract mode. After the ball begins,
   then the playfield needs enabling. Each of these situations requires a key combination.
- mach2/a can start by pressing ASX together.
- vrnwrld can start by pressing ASDX together.
- jolypark can start by pressing ASDX together.

ToDo:
- Multiball games - find out the key combinations
- Bushidoa non-functional (bad maincpu rom?)
- Sounds are running at half speed.

****************************************************************************************************/

#include "emu.h"
#include "machine/genpin.h"

#include "cpu/mcs51/mcs51.h"
#include "cpu/z80/z80.h"
#include "machine/74157.h"
#include "machine/7474.h"
#include "machine/i8255.h"
#include "sound/msm5205.h"

#include "emupal.h"
#include "screen.h"
#include "speaker.h"
#include "metalman.lh"

namespace {

class spinb_state : public genpin_class
{
public:
	spinb_state(const machine_config &mconfig, device_type type, const char *tag)
		: genpin_class(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_musiccpu(*this, "musiccpu")
		, m_dmdcpu(*this, "dmdcpu")
		, m_p_audio(*this, "audiorom")
		, m_p_music(*this, "musicrom")
		, m_p_dmdcpu(*this, "dmdcpu")
		, m_msm_a(*this, "msm_a")
		, m_msm_m(*this, "msm_m")
		, m_ic5a(*this, "ic5a")
		, m_ic5m(*this, "ic5m")
		, m_ic14a(*this, "ic14a")
		, m_ic14m(*this, "ic14m")
		, m_io_keyboard(*this, "X%d", 0U)
		, m_digits(*this, "digit%d", 0U)
		, m_io_outputs(*this, "out%d", 0U)
	{ }

	void jolypark(machine_config &config);
	void vrnwrld(machine_config &config);
	void spinb(machine_config &config);
	void metalman(machine_config &config);

	void init_0() { m_game = 0; }
	void init_1() { m_game = 1; }
	void init_2() { m_game = 2; }
	void init_3() { m_game = 3; }

private:
	virtual void machine_reset() override;
	virtual void machine_start() override;
	void p1_w(u8 data);
	u8 p3_r();
	void p3_w(u8 data);
	u8 ppia_c_r();
	void ppia_a_w(u8 data);
	void ppia_b_w(u8 data);
	void ppia_c_w(u8 data);
	u8 ppim_c_r();
	u8 ppi60b_r() { return BIT(m_return_status, 4) ? 0 : 2; }  // metalman
	u8 ppi64c_r();
	void ppim_a_w(u8 data);
	void ppim_b_w(u8 data);
	void ppim_c_w(u8 data);
	void ppi60a_w(u8 data);
	void ppi60b_w(u8 data);
	void ppi64a_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[i] = BIT(data, i); }
	void ppi64b_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[8U+i] = BIT(data, i); }
	void ppi64c_w(u8 data); // metalman
	void ppi68a_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[16U+i] = BIT(data, i); }
	void ppi68b_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[24U+i] = BIT(data, i) ^ 1; }
	void ppi68c_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[32U+i] = BIT(data, i); }
	void ppi6ca_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[40U+i] = BIT(data, i); }
	void ppi6cb_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[48U+i] = BIT(data, i); }
	void ppi6cc_w(u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[56U+i] = BIT(data, i); }
	void extras_w(offs_t offset, u8 data) { for (u8 i = 0; i < 8; i++) m_io_outputs[64U+offset*8+i] = BIT(data, i); }
	u8 sw_r();
	void dmdram_w(offs_t offset, u8 data);
	u8 dmdram_r(offs_t offset);
	u8 sndcmd_r();
	void sndbank_a_w(u8 data);
	void sndbank_m_w(u8 data);
	void sndcmd_w(u8 data);
	void volume_w(u8 data) { };
	void disp_w(offs_t, u8 data);
	void update_sound_a();
	void update_sound_m();
	DECLARE_WRITE_LINE_MEMBER(ic5a_w);
	DECLARE_WRITE_LINE_MEMBER(ic5m_w);
	void spinb_palette(palette_device &palette) const;

	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void dmd_io(address_map &map);
	void dmd_mem(address_map &map);
	void audio_map(address_map &map);
	void spinb_map(address_map &map);
	void music_map(address_map &map);
	void vrnwrld_map(address_map &map);

	bool m_pc0a = 0;
	bool m_pc0m = 0;
	u8 m_game = 0U;
	u8 m_row = 0U;
	u8 m_p3 = 0U;
	u8 m_p32 = 0U;
	u8 m_dmdcmd = 0U;
	u8 m_dmdbank = 0U;
	u8 m_dmdextaddr = 0U;
	u8 m_dmdram[0x2000]{};
	u8 m_sndcmd = 0U;
	u8 m_sndbank_a = 0U;
	u8 m_sndbank_m = 0U;
	u32 m_sound_addr_a = 0U;
	u32 m_sound_addr_m = 0U;
	u32 m_audio_size = 0U;
	u32 m_music_size = 0U;
	u32 m_dmd_size = 0U;
	u8 m_return_status = 0U;
	u8 m_segment[8]{}; // metalman
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<cpu_device> m_musiccpu;
	optional_device<i8031_device> m_dmdcpu;
	required_region_ptr<u8> m_p_audio;
	required_region_ptr<u8> m_p_music;
	required_region_ptr<u8> m_p_dmdcpu;
	required_device<msm5205_device> m_msm_a;
	required_device<msm5205_device> m_msm_m;
	required_device<ttl7474_device> m_ic5a;
	required_device<ttl7474_device> m_ic5m;
	required_device<hc157_device> m_ic14a;
	required_device<hc157_device> m_ic14m;
	required_ioport_array<10> m_io_keyboard;
	output_finder<50> m_digits;
	output_finder<112> m_io_outputs;
};

void spinb_state::spinb_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x5fff).ram().share("nvram"); // 6164, battery-backed
	map(0x6000, 0x6003).mirror(0x13fc).rw("ppi60", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6400, 0x6403).mirror(0x13fc).rw("ppi64", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6800, 0x6803).mirror(0x13fc).rw("ppi68", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6c00, 0x6c03).mirror(0x131c).rw("ppi6c", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6c20, 0x6c3f).mirror(0x1300).w(FUNC(spinb_state::sndcmd_w));
	map(0x6c40, 0x6c45).mirror(0x1300).w(FUNC(spinb_state::extras_w));
	map(0x6c60, 0x6c67).mirror(0x1300).w(FUNC(spinb_state::disp_w));
	map(0x6ce0, 0x6ce0).nopw();
}

void spinb_state::vrnwrld_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x9fff).ram().share("nvram"); // 6164, battery-backed
	map(0xc000, 0xc003).mirror(0x13fc).rw("ppi60", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xc400, 0xc403).mirror(0x13fc).rw("ppi64", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xc800, 0xc803).mirror(0x13fc).rw("ppi68", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xcc00, 0xcc03).mirror(0x131c).rw("ppi6c", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xcc20, 0xcc3f).mirror(0x1300).w(FUNC(spinb_state::sndcmd_w));
	map(0xcc40, 0xcc45).mirror(0x1300).w(FUNC(spinb_state::extras_w));
	map(0xcc60, 0xcc60).mirror(0x1300).w(FUNC(spinb_state::disp_w));
	map(0xcce0, 0xcce0).nopw();
}

void spinb_state::audio_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x3fff).ram(); // 6164
	map(0x4000, 0x4003).mirror(0x1ffc).rw("ppia", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6000, 0x6000).w(FUNC(spinb_state::sndbank_a_w));
	map(0x8000, 0x8000).r(FUNC(spinb_state::sndcmd_r));
}

void spinb_state::music_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x3fff).ram(); // 6164
	map(0x4000, 0x4003).mirror(0x1ffc).rw("ppim", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x6000, 0x6000).w(FUNC(spinb_state::sndbank_m_w));
	map(0x8000, 0x8000).r(FUNC(spinb_state::sndcmd_r));
	map(0xA000, 0xA000).w(FUNC(spinb_state::volume_w));
}

void spinb_state::dmd_mem(address_map &map)
{
	map(0x0000, 0xffff).rom();
}

void spinb_state::dmd_io(address_map &map)
{
	map(0x0000, 0x1fff).w(FUNC(spinb_state::dmdram_w));
	map(0x0000, 0xffff).r(FUNC(spinb_state::dmdram_r));
}

static INPUT_PORTS_START( spinb )
	PORT_START("X0")
	PORT_DIPNAME( 0x01, 0x00, "Coin control 1")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x01, "On")
	PORT_DIPNAME( 0x02, 0x00, "Coin control 2")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x02, "On")
	PORT_DIPNAME( 0x04, 0x00, "Coin control 3")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x04, "On")
	PORT_DIPNAME( 0x08, 0x00, "Coin control 4")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x08, "On")
	PORT_DIPNAME( 0x30, 0x00, "Points for free game")
	PORT_DIPSETTING(    0x00, "300 million")
	PORT_DIPSETTING(    0x10, "400 million")
	PORT_DIPSETTING(    0x20, "500 million")
	PORT_DIPSETTING(    0x30, "600 million")
	PORT_DIPNAME( 0x80, 0x00, "Selection Mode")
	PORT_DIPSETTING(    0x00, "Normal")
	PORT_DIPSETTING(    0x80, "Exhibition")

	PORT_START("X1")
	PORT_DIPNAME( 0x08, 0x00, "Balls")
	PORT_DIPSETTING(    0x00, "3")
	PORT_DIPSETTING(    0x08, "5")
	PORT_DIPNAME( 0x03, 0x00, "High Score") //"Handicap"
	PORT_DIPSETTING(    0x00, "700 million")
	PORT_DIPSETTING(    0x01, "750 million")
	PORT_DIPSETTING(    0x02, "800 million")
	PORT_DIPSETTING(    0x03, "850 million")
	PORT_DIPNAME( 0x30, 0x00, "Time for New Ticket")
	PORT_DIPSETTING(    0x00, "None")
	PORT_DIPSETTING(    0x10, "Short")
	PORT_DIPSETTING(    0x20, "Medium")
	PORT_DIPSETTING(    0x30, "Long")

	PORT_START("X2")
	PORT_DIPNAME( 0x08, 0x00, "Automatic Punctuation")
	PORT_DIPSETTING(    0x00, DEF_STR(No))
	PORT_DIPSETTING(    0x08, DEF_STR(Yes))
	PORT_DIPNAME( 0x03, 0x00, "Laps for extra ball in looping")
	PORT_DIPSETTING(    0x00, "0")
	PORT_DIPSETTING(    0x01, "1")
	PORT_DIPSETTING(    0x02, "2")
	PORT_DIPSETTING(    0x03, "3")
	PORT_DIPNAME( 0x30, 0x00, "Time for 3-ball multiball")
	PORT_DIPSETTING(    0x00, "None")
	PORT_DIPSETTING(    0x10, "Short")
	PORT_DIPSETTING(    0x20, "Medium")
	PORT_DIPSETTING(    0x30, "Long")
	PORT_DIPNAME( 0x40, 0x00, "Difficulty of Special")
	PORT_DIPSETTING(    0x00, "Easy")
	PORT_DIPSETTING(    0x40, "Hard")

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) // "Monedero A"
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) // "Monedero B"
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) // "Monedero C"
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_9) PORT_NAME("Tilt") // "Falta"
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 ) // "Pulsador Partidas"
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Reset") // "Puesta a cero"
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Accounting info") // "Test economico"
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_0_PAD) PORT_NAME("Test") // "Test tecnico"
	PORT_BIT( 0x100, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Test Contactos")

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_B) PORT_NAME("INP40")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_C) PORT_NAME("INP41")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_E) PORT_NAME("INP42")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_F) PORT_NAME("INP43")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_G) PORT_NAME("INP44")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_H) PORT_NAME("INP45")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_I) PORT_NAME("INP46")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_J) PORT_NAME("INP47")

	PORT_START("X5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_K) PORT_NAME("INP50")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_L) PORT_NAME("INP51")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_M) PORT_NAME("INP52")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_N) PORT_NAME("INP53")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_O) PORT_NAME("INP54")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_P) PORT_NAME("INP55")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Q) PORT_NAME("INP56")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_R) PORT_NAME("INP57")

	PORT_START("X6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_T) PORT_NAME("INP60")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_U) PORT_NAME("INP61")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_V) PORT_NAME("INP62")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_W) PORT_NAME("INP63")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Y) PORT_NAME("INP64")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_COMMA) PORT_NAME("INP65")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_STOP) PORT_NAME("INP66")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_SLASH) PORT_NAME("INP67")

	PORT_START("X7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_X) PORT_NAME("INP70") // 4th stored ball
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_A) PORT_NAME("INP71") // 3rd stored ball
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_S) PORT_NAME("INP72") // 2nd stored ball
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_D) PORT_NAME("INP73") // first stored ball
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Z) PORT_NAME("INP74") // At shooter
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_COLON) PORT_NAME("INP75")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_QUOTE) PORT_NAME("INP76")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_ENTER) PORT_NAME("INP77")

	PORT_START("X8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_NAME("INP80")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_NAME("INP81")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_BACKSLASH) PORT_NAME("INP82")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_MINUS) PORT_NAME("INP83")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_EQUALS) PORT_NAME("INP84")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_BACKSPACE) PORT_NAME("INP85")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("INP86")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_DEL) PORT_NAME("INP87")

	PORT_START("X9")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_UP) PORT_NAME("INP90")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_LEFT) PORT_NAME("INP91")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_RIGHT) PORT_NAME("INP92")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_DOWN) PORT_NAME("INP93")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_HOME) PORT_NAME("INP94")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_END) PORT_NAME("INP95")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_PGUP) PORT_NAME("INP96")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_PGDN) PORT_NAME("INP97")
INPUT_PORTS_END

static INPUT_PORTS_START( vrnwrld )
	PORT_INCLUDE( spinb )
	PORT_MODIFY("X1")
	PORT_DIPNAME( 0x30, 0x00, "Time for New Travel")
	PORT_DIPSETTING(    0x00, "None")
	PORT_DIPSETTING(    0x10, "Short")
	PORT_DIPSETTING(    0x20, "Medium")
	PORT_DIPSETTING(    0x30, "Long")

	PORT_MODIFY("X2")
	PORT_DIPNAME( 0x03, 0x00, "Difficulty")
	PORT_DIPSETTING(    0x00, "Very Easy")
	PORT_DIPSETTING(    0x01, "Easy")
	PORT_DIPSETTING(    0x02, "Hard")
	PORT_DIPSETTING(    0x03, "Very Hard")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( mach2 )
	PORT_INCLUDE( spinb )
	PORT_MODIFY("X0")
	PORT_DIPNAME( 0x30, 0x00, "Points for free game")
	PORT_DIPSETTING(    0x00, "100 million")
	PORT_DIPSETTING(    0x10, "150 million")
	PORT_DIPSETTING(    0x20, "200 million")
	PORT_DIPSETTING(    0x30, "250 million")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("X1")
	PORT_DIPNAME( 0x03, 0x00, "High Score") //"Handicap"
	PORT_DIPSETTING(    0x00, "250 million")
	PORT_DIPSETTING(    0x01, "300 million")
	PORT_DIPSETTING(    0x02, "350 million")
	PORT_DIPSETTING(    0x03, "400 million")
	PORT_DIPNAME( 0x30, 0x00, "Time for New Shot")
	PORT_DIPSETTING(    0x00, "None")
	PORT_DIPSETTING(    0x10, "10 seconds")
	PORT_DIPSETTING(    0x20, "12 seconds")
	PORT_DIPSETTING(    0x30, "15 seconds")
	PORT_DIPNAME( 0xc0, 0x00, "Laps to activate ramp")
	PORT_DIPSETTING(    0x00, "1")
	PORT_DIPSETTING(    0x40, "2")
	PORT_DIPSETTING(    0x80, "3")
	PORT_DIPSETTING(    0xc0, "4")

	PORT_MODIFY("X2")
	PORT_DIPNAME( 0x04, 0x00, "Aeroplane Special")
	PORT_DIPSETTING(    0x00, DEF_STR(No))
	PORT_DIPSETTING(    0x04, DEF_STR(Yes))
	PORT_DIPNAME( 0x01, 0x00, "Lock")
	PORT_DIPSETTING(    0x00, "Easy")
	PORT_DIPSETTING(    0x01, "Hard")
	PORT_DIPNAME( 0x80, 0x00, "Rescue")
	PORT_DIPSETTING(    0x00, "Easy")
	PORT_DIPSETTING(    0x80, "Hard")
	PORT_BIT( 0x72, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("X7")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Z) PORT_NAME("INP73") // At shooter
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_D) PORT_NAME("INP74")
INPUT_PORTS_END

static INPUT_PORTS_START( bushido )
	PORT_INCLUDE( mach2 )
	PORT_MODIFY("X1")
	PORT_DIPNAME( 0x30, 0x00, "Time for Shoot Again")
	PORT_DIPSETTING(    0x00, "8 seconds")
	PORT_DIPSETTING(    0x10, "10 seconds")
	PORT_DIPSETTING(    0x20, "12 seconds")
	PORT_DIPSETTING(    0x30, "15 seconds")
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("X2")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("X3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_D) PORT_NAME("INP31") // first stored ball
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_LEFT) PORT_NAME("INP32")

	PORT_MODIFY("X6")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_S) PORT_NAME("INP64") // 2nd stored ball

	PORT_MODIFY("X7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_A) PORT_NAME("INP70") // 3rd stored ball
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_X) PORT_NAME("INP71") // outhole
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Z) PORT_NAME("INP72") // At shooter
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Y) PORT_NAME("INP73")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_UP) PORT_NAME("INP74")

	PORT_MODIFY("X9")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( metalman )
	PORT_START("X0")
	PORT_DIPNAME( 0x01, 0x00, "Coin control 1")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x01, "On")
	PORT_DIPNAME( 0x02, 0x00, "Coin control 2")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x02, "On")
	PORT_DIPNAME( 0x04, 0x00, "Coin control 3")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x04, "On")
	PORT_DIPNAME( 0x08, 0x00, "Coin control 4")
	PORT_DIPSETTING(    0x00, "Off")
	PORT_DIPSETTING(    0x08, "On")
	PORT_DIPNAME( 0x30, 0x00, "Points for free game")
	PORT_DIPSETTING(    0x00, "3600000")
	PORT_DIPSETTING(    0x10, "4000000")
	PORT_DIPSETTING(    0x20, "4400000")
	PORT_DIPSETTING(    0x30, "4800000")

	PORT_START("X1")
	PORT_DIPNAME( 0x08, 0x00, "Balls")
	PORT_DIPSETTING(    0x00, "3")
	PORT_DIPSETTING(    0x08, "5")
	PORT_DIPNAME( 0x03, 0x00, "High Score") //"Handicap"
	PORT_DIPSETTING(    0x00, "5600000")
	PORT_DIPSETTING(    0x01, "6000000")
	PORT_DIPSETTING(    0x02, "6400000")
	PORT_DIPSETTING(    0x03, "6800000")
	PORT_DIPNAME( 0x10, 0x00, "Dificultad de Especiales")
	PORT_DIPSETTING(    0x00, "Varios por partida")
	PORT_DIPSETTING(    0x10, "Solo uno por partida")

	PORT_START("X2") // not on the pcb

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) // "Monedero A"
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_A) PORT_NAME("INP32")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_9) PORT_NAME("Tilt") // "Falta"
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 ) // "Pulsador Partidas"
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Reset") // "Puesta a cero"
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Accounting info") // "Test economico"
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_0_PAD) PORT_NAME("Test") // "Test tecnico"

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_B) PORT_NAME("INP40")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_C) PORT_NAME("INP41")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_D) PORT_NAME("INP42")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_E) PORT_NAME("INP43")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_F) PORT_NAME("INP44")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_G) PORT_NAME("INP45")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_H) PORT_NAME("INP46")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_I) PORT_NAME("INP47")

	PORT_START("X5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_J) PORT_NAME("INP50")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_K) PORT_NAME("INP51")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_L) PORT_NAME("INP52")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_M) PORT_NAME("INP53")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_N) PORT_NAME("INP54")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_O) PORT_NAME("INP55")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_P) PORT_NAME("INP56")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Q) PORT_NAME("INP57")

	PORT_START("X6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_R) PORT_NAME("INP60")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_S) PORT_NAME("INP61")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_T) PORT_NAME("INP62")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_U) PORT_NAME("INP63")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_V) PORT_NAME("INP64")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_W) PORT_NAME("INP65")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Y) PORT_NAME("INP66")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_Z) PORT_NAME("INP67")

	PORT_START("X7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_COMMA) PORT_NAME("INP70")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_X) PORT_NAME("Outhole")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_STOP) PORT_NAME("INP72")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_SLASH) PORT_NAME("INP74")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_COLON) PORT_NAME("INP75")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_QUOTE) PORT_NAME("INP76")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_CODE(KEYCODE_ENTER) PORT_NAME("INP77")

	PORT_START("X8")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("X9")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

//Metalman
void spinb_state::ppi64c_w(u8 data)
{
	if (BIT(data, 3))
	{
		data &= 7;
		for (u8 i = 0; i < 5; i++)
			m_digits[i*10+data] = m_segment[i];
	}
}

/*
d0 = / enable RAM
d1 = display enable
d2 = RDATA    )
d3 = ROWCK    ) to DMD
d4 = COLATCH  )
d5 = part of the data input circuit
d6 = STAT0
d7 = STAT1

m_game == 0 : P1.0 low for RAM, P1.5 low for data input. They shouldn't both be low. (0=bad; 1=input; 2=ram; 3=none)
m_game == 1 : P1.0 low for RAM, P1.5 low for data input. They shouldn't both be low. Extra ROM selected by P3.4
m_game == 2 : P1.0 and P1.5 go to 74LS139 selector: 0 = RAM; 1 = ROM1; 2 = ROM2; 3 = data input
*/
void spinb_state::p1_w(u8 data)
{
	m_return_status = (m_return_status & 0xfc) | BIT(data, 6, 2);
	m_dmdbank = (BIT(data, 5) << 1) + BIT(data, 0); // vrnwrld

	// map the other games into vrnwrld's arrangement
	if (m_game < 2)
	{
		switch (m_dmdbank)
		{
			case 0:
				printf("DMD Illegal selection\n");
				break;
			case 1: // input
				m_dmdbank = 3;
				break;
			case 2: // ram
				m_dmdbank = 0;
				break;
			case 3: // nothing or (game1 external rom)
				m_dmdbank = 1;
				break;
		}
	}
}

u8 spinb_state::p3_r()
{
	return m_p3 | m_p32;
}

/*
d0 = RXD - SDATA ) to DMD
d1 = TXD - DOTCK )
d2 = Interrupt Input when data is coming from maincpu
d6 = External /WR
d7 = External /RD
*/
void spinb_state::p3_w(u8 data)
{
	m_p3 = data & 0xfb;
	m_dmdextaddr = 0;

	if (m_game == 1)
		m_dmdextaddr = BIT(data, 3);// | (BIT(data, 5) << 1);
	else
	if (m_game == 2)
		m_dmdextaddr = BIT(data, 3) | (BIT(data, 5) << 1) | (BIT(data, 4) << 2);
}

u8 spinb_state::sw_r()
{
	if (m_row < 10)
		return m_io_keyboard[m_row]->read();
	else
		return 0;
}

void spinb_state::sndcmd_w(u8 data)
{
	m_sndcmd = data;
}

u8 spinb_state::sndcmd_r()
{
	return m_sndcmd;
}

void spinb_state::dmdram_w(offs_t offset, u8 data)
{
	m_dmdram[offset & 0x1fff] = data;
}

u8 spinb_state::dmdram_r(offs_t offset)
{
	u32 t;
	switch (m_dmdbank)
	{
		case 0:
			return m_dmdram[offset & 0x1fff];
		case 1:
			t = 0x10000 + offset + (m_dmdextaddr << 16);
			if (t < m_dmd_size)
				return m_p_dmdcpu[t];
			else
				return m_p_dmdcpu[offset];
		case 2:
			t = 0x90000 + offset + (m_dmdextaddr << 16);
			if (t < m_dmd_size)
				return m_p_dmdcpu[t];
			else
				return m_p_dmdcpu[offset];
	}

	m_p32 = 4;
	m_dmdcpu->set_input_line(MCS51_INT0_LINE, CLEAR_LINE);
	m_return_status &= 0xfb;
	return m_dmdcmd;
}

void spinb_state::disp_w(offs_t offset, u8 data)
{
	if (m_game == 3)
		m_segment[offset] = data;
	else
	{
		m_dmdcmd = data;
		m_p32 = 0;
		m_return_status |= 4;
		m_dmdcpu->set_input_line(MCS51_INT0_LINE, HOLD_LINE);
	}
}

void spinb_state::ppi60a_w(u8 data)
{
	if (data)
		for (u8 i = 0; i < 8; i++)
			if (BIT(data, i))
				m_row = i;
}

void spinb_state::ppi60b_w(u8 data)
{
	if (data & 7)
		for (u8 i = 0; i < 3; i++)
			if (BIT(data, i))
				m_row = i+8;
}

u8 spinb_state::ppi64c_r()
{
	u8 data = BIT(m_io_keyboard[3]->read(), 8);
	return m_return_status | (data << 3);
}

void spinb_state::sndbank_a_w(u8 data)
{
	m_sndbank_a = data;
	m_sound_addr_a = (m_sound_addr_a & 0xffff) | ((data & 7) << 16);
	switch(BIT(data, 5, 3))
	{
		case 3:
			break;
		case 5:
			m_sound_addr_a |= (1<<19);
			break;
		case 6:
			m_sound_addr_a |= (2<<19);
			break;
		default:
			m_sndbank_a = 0xff;
			break;
	}

	update_sound_a();
}

void spinb_state::sndbank_m_w(u8 data)
{
	m_sndbank_m = data;
	m_sound_addr_m = (m_sound_addr_m & 0xffff) | ((data & 7) << 16);

	switch(BIT(data, 5, 3))
	{
		case 3:
			break;
		case 5:
			m_sound_addr_m |= (1<<19);
			break;
		case 6:
			m_sound_addr_m |= (2<<19);
			break;
		default:
			m_sndbank_m = 0xff;
			break;
	}

	update_sound_m();
}

void spinb_state::update_sound_a()
{
	if ((m_sound_addr_a < m_audio_size) && (m_sndbank_a != 0xff))
		m_ic14a->ba_w(m_p_audio[m_sound_addr_a]);
	else
		m_ic14a->ba_w(0);
}

void spinb_state::update_sound_m()
{
	if ((m_sound_addr_m < m_music_size) && (m_sndbank_m != 0xff))
		m_ic14m->ba_w(m_p_music[m_sound_addr_m]);
	else
		m_ic14m->ba_w(0);
}

WRITE_LINE_MEMBER( spinb_state::ic5a_w )
{
	m_pc0a = state;
	m_ic5a->d_w(state);
	m_ic14a->select_w(state);
}

WRITE_LINE_MEMBER( spinb_state::ic5m_w )
{
	m_pc0m = state;
	m_ic5m->d_w(state);
	m_ic14m->select_w(state);
}

u8 spinb_state::ppia_c_r()
{
	return m_pc0a ? 15 : 14;
}

u8 spinb_state::ppim_c_r()
{
	return m_pc0m ? 15 : 14;
}

void spinb_state::ppia_b_w(u8 data)
{
	m_sound_addr_a = (m_sound_addr_a & 0xffff00) | data;
	update_sound_a();
}

void spinb_state::ppim_b_w(u8 data)
{
	m_sound_addr_m = (m_sound_addr_m & 0xffff00) | data;
	update_sound_m();
}

void spinb_state::ppia_a_w(u8 data)
{
	m_sound_addr_a = (m_sound_addr_a & 0xff00ff) | (data << 8);
	update_sound_a();
}

void spinb_state::ppim_a_w(u8 data)
{
	m_sound_addr_m = (m_sound_addr_m & 0xff00ff) | (data << 8);
	update_sound_m();
}

void spinb_state::ppia_c_w(u8 data)
{
	m_return_status = (m_return_status & 0xef) | (data & 0x10);
	m_msm_a->s1_w(BIT(data, 5));
	m_msm_a->reset_w(BIT(data, 6));
	m_ic5a->clear_w(!BIT(data, 6));
}

void spinb_state::ppim_c_w(u8 data)
{
	// pc4 - READY line back to cpu board, but not used
	m_msm_m->s1_w(BIT(data, 5));
	m_msm_m->reset_w(BIT(data, 6));
	m_ic5m->clear_w(!BIT(data, 6));
}

void spinb_state::machine_start()
{
	genpin_class::machine_start();
	m_digits.resolve();
	m_io_outputs.resolve();

	m_dmd_size = memregion("dmdcpu")->bytes();
	m_audio_size = memregion("audiorom")->bytes();
	m_music_size = memregion("musicrom")->bytes();

	save_item(NAME(m_pc0a));
	save_item(NAME(m_pc0m));
	save_item(NAME(m_game));
	save_item(NAME(m_row));
	save_item(NAME(m_p3));
	save_item(NAME(m_p32));
	save_item(NAME(m_dmdcmd));
	save_item(NAME(m_dmdbank));
	save_item(NAME(m_dmdextaddr));
	save_item(NAME(m_dmdram));
	save_item(NAME(m_sndcmd));
	save_item(NAME(m_sndbank_a));
	save_item(NAME(m_sndbank_m));
	save_item(NAME(m_sound_addr_a));
	save_item(NAME(m_sound_addr_m));
	save_item(NAME(m_audio_size));
	save_item(NAME(m_music_size));
	save_item(NAME(m_dmd_size));
	save_item(NAME(m_return_status));
	save_item(NAME(m_segment));
}

void spinb_state::machine_reset()
{
	genpin_class::machine_reset();
	for (u8 i = 0; i < m_io_outputs.size(); i++)
		m_io_outputs[i] = 0;

	m_sound_addr_a = 0;
	m_sound_addr_m = 0;
	m_sndbank_a = 0xff;
	m_sndbank_m = 0xff;
	m_row = 0;
	update_sound_a();
	update_sound_m();
}

void  spinb_state::spinb_palette(palette_device &palette) const
{
	palette.set_pen_color(0, rgb_t(0x00, 0x00, 0x00));
	palette.set_pen_color(1, rgb_t(0xf7, 0xaa, 0x00));
	palette.set_pen_color(2, rgb_t(0x7c, 0x55, 0x00));
}

u32 spinb_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	u16 sy=0,ma;
	address_space &internal = m_dmdcpu->space(AS_DATA);
	ma = internal.read_byte(0x05) << 8; // find where display memory is

	if (m_game)
	{
		ma = ((ma - 0x200) & 0x1c00) + 0x200;
		if (ma > 0x1c00) return 1; // not initialised yet

		for (u8 y = 0; y < 32; y++)
		{
			u16 *p = &bitmap.pix(sy++);
			for (uint16_t x = 0; x < 16; x++)
			{
				u8 const gfx = m_dmdram[ma+0x200];
				u8 const gfx1 = m_dmdram[ma++];

				*p++ = BIT(gfx1, 0) ? 1 : BIT(gfx, 0) ? 2 : 0;
				*p++ = BIT(gfx1, 1) ? 1 : BIT(gfx, 1) ? 2 : 0;
				*p++ = BIT(gfx1, 2) ? 1 : BIT(gfx, 2) ? 2 : 0;
				*p++ = BIT(gfx1, 3) ? 1 : BIT(gfx, 3) ? 2 : 0;
				*p++ = BIT(gfx1, 4) ? 1 : BIT(gfx, 4) ? 2 : 0;
				*p++ = BIT(gfx1, 5) ? 1 : BIT(gfx, 5) ? 2 : 0;
				*p++ = BIT(gfx1, 6) ? 1 : BIT(gfx, 6) ? 2 : 0;
				*p++ = BIT(gfx1, 7) ? 1 : BIT(gfx, 7) ? 2 : 0;
			}
		}
	}
	else
	{
		ma &= 0x1e00;

		for (u8 y = 0; y < 32; y++)
		{
			u16 *p = &bitmap.pix(sy++);
			for (uint16_t x = 0; x < 16; x++)
			{
				u8 const gfx = m_dmdram[ma++];

				*p++ = BIT(gfx, 0);
				*p++ = BIT(gfx, 1);
				*p++ = BIT(gfx, 2);
				*p++ = BIT(gfx, 3);
				*p++ = BIT(gfx, 4);
				*p++ = BIT(gfx, 5);
				*p++ = BIT(gfx, 6);
				*p++ = BIT(gfx, 7);
			}
		}
	}
	return 0;
}

void spinb_state::spinb(machine_config &config)
{
	// Basic machine hardware
	Z80(config, m_maincpu, XTAL(5'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &spinb_state::spinb_map);
	m_maincpu->set_periodic_int(FUNC(spinb_state::irq0_line_hold), attotime::from_hz(175)); // NE556

	Z80(config, m_audiocpu, XTAL(5'000'000) / 2);
	m_audiocpu->set_addrmap(AS_PROGRAM, &spinb_state::audio_map);

	Z80(config, m_musiccpu, XTAL(5'000'000) / 2);
	m_musiccpu->set_addrmap(AS_PROGRAM, &spinb_state::music_map);

	I8031(config, m_dmdcpu, XTAL(16'000'000));
	m_dmdcpu->set_addrmap(AS_PROGRAM, &spinb_state::dmd_mem);
	m_dmdcpu->set_addrmap(AS_IO, &spinb_state::dmd_io);
	m_dmdcpu->port_out_cb<1>().set(FUNC(spinb_state::p1_w));
	m_dmdcpu->port_in_cb<3>().set(FUNC(spinb_state::p3_r));
	m_dmdcpu->port_out_cb<3>().set(FUNC(spinb_state::p3_w));

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);

	// Video
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500));
	screen.set_screen_update(FUNC(spinb_state::screen_update));
	screen.set_size(128, 32);
	screen.set_visarea(0, 127, 0, 31);
	screen.set_palette("palette");

	PALETTE(config, "palette", FUNC(spinb_state::spinb_palette), 3);

	// Sound
	genpin_audio(config);

	SPEAKER(config, "msmavol").front_center();
	MSM5205(config, m_msm_a, XTAL(384'000));
	m_msm_a->vck_callback().set("ic5a", FUNC(ttl7474_device::clock_w));
	m_msm_a->set_prescaler_selector(msm5205_device::S96_4B); // 4KHz 4-bit
	m_msm_a->add_route(ALL_OUTPUTS, "msmavol", 1.0);

	SPEAKER(config, "msmmvol").front_center();
	MSM5205(config, m_msm_m, XTAL(384'000));
	m_msm_m->vck_callback().set("ic5m", FUNC(ttl7474_device::clock_w));
	m_msm_m->set_prescaler_selector(msm5205_device::S96_4B); // 4KHz 4-bit
	m_msm_m->add_route(ALL_OUTPUTS, "msmmvol", 1.0);

	// Devices
	i8255_device &ppi60(I8255A(config, "ppi60"));
	//ppi60.in_pa_callback().set(FUNC(spinb_state::ppi60a_r));
	ppi60.out_pa_callback().set(FUNC(spinb_state::ppi60a_w));
	ppi60.in_pb_callback().set(FUNC(spinb_state::ppi60b_r));
	ppi60.out_pb_callback().set(FUNC(spinb_state::ppi60b_w));
	ppi60.in_pc_callback().set(FUNC(spinb_state::sw_r));
	//ppi60.out_pc_callback().set(FUNC(spinb_state::ppi60c_w));

	i8255_device &ppi64(I8255A(config, "ppi64"));
	ppi64.in_pc_callback().set(FUNC(spinb_state::ppi64c_r));
	ppi64.out_pa_callback().set(FUNC(spinb_state::ppi64a_w));
	ppi64.out_pb_callback().set(FUNC(spinb_state::ppi64b_w));
	ppi64.out_pc_callback().set(FUNC(spinb_state::ppi64c_w));

	i8255_device &ppi68(I8255A(config, "ppi68"));
	ppi68.out_pa_callback().set(FUNC(spinb_state::ppi68a_w));
	ppi68.out_pb_callback().set(FUNC(spinb_state::ppi68b_w));
	ppi68.out_pc_callback().set(FUNC(spinb_state::ppi68c_w));

	i8255_device &ppi6c(I8255A(config, "ppi6c"));
	ppi6c.out_pa_callback().set(FUNC(spinb_state::ppi6ca_w));
	ppi6c.out_pb_callback().set(FUNC(spinb_state::ppi6cb_w));
	ppi6c.out_pc_callback().set(FUNC(spinb_state::ppi6cc_w));

	i8255_device &ppia(I8255A(config, "ppia"));
	ppia.out_pa_callback().set(FUNC(spinb_state::ppia_a_w));
	ppia.out_pb_callback().set(FUNC(spinb_state::ppia_b_w));
	ppia.in_pc_callback().set(FUNC(spinb_state::ppia_c_r));
	ppia.out_pc_callback().set(FUNC(spinb_state::ppia_c_w));

	i8255_device &ppim(I8255A(config, "ppim"));
	ppim.out_pa_callback().set(FUNC(spinb_state::ppim_a_w));
	ppim.out_pb_callback().set(FUNC(spinb_state::ppim_b_w));
	ppim.in_pc_callback().set(FUNC(spinb_state::ppim_c_r));
	ppim.out_pc_callback().set(FUNC(spinb_state::ppim_c_w));

	TTL7474(config, m_ic5a, 0);
	m_ic5a->comp_output_cb().set(FUNC(spinb_state::ic5a_w));

	HC157(config, m_ic14a, 0); // IC15 on Jolly Park
	m_ic14a->out_callback().set("msm_a", FUNC(msm5205_device::data_w));

	TTL7474(config, m_ic5m, 0);
	m_ic5m->comp_output_cb().set(FUNC(spinb_state::ic5m_w));

	HC157(config, m_ic14m, 0); // IC15 on Jolly Park
	m_ic14m->out_callback().set("msm_m", FUNC(msm5205_device::data_w));
}

void spinb_state::jolypark(machine_config &config)
{
	spinb(config);

	MSM6585(config.replace(), m_msm_a, XTAL(640'000));
	m_msm_a->vck_callback().set("ic5a", FUNC(ttl7474_device::clock_w));
	m_msm_a->set_prescaler_selector(msm6585_device::S160); // 4KHz 4-bit
	m_msm_a->add_route(ALL_OUTPUTS, "msmavol", 1.0);

	MSM6585(config.replace(), m_msm_m, XTAL(640'000));
	m_msm_m->vck_callback().set("ic5m", FUNC(ttl7474_device::clock_w));
	m_msm_m->set_prescaler_selector(msm6585_device::S160); // 4KHz 4-bit
	m_msm_m->add_route(ALL_OUTPUTS, "msmmvol", 1.0);
}

void spinb_state::vrnwrld(machine_config &config)
{
	jolypark(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &spinb_state::vrnwrld_map);
}

void spinb_state::metalman(machine_config &config)
{
	spinb(config);
	config.device_remove("dmdcpu");
	config.device_remove("screen");
	config.device_remove("palette");
	config.set_default_layout(layout_metalman);
}

/*-------------------------------------------------------------------
/ Metal Man (1992)
/-------------------------------------------------------------------*/
ROM_START(metalman)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("cpu_0.bin", 0x0000, 0x2000, CRC(7fe4335b) SHA1(52ef2efa29337eebd8c2c9a8aec864356a6829b6))
	ROM_LOAD("cpu_1.bin", 0x2000, 0x2000, CRC(2cca735e) SHA1(6a76017dfbcac0d57fcec8f07f92d5e04dd3e00b))

	ROM_REGION(0x10000, "dmdcpu", ROMREGION_ERASEFF)

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("sound_e1.bin", 0x0000, 0x2000, CRC(55e889e8) SHA1(0a240868c1b17762588c0ed9a14f568a6e50f409))

	ROM_REGION(0x60000, "audiorom", ROMREGION_ERASEFF)
	ROM_LOAD("sound_e2.bin", 0x40000, 0x20000, CRC(5ac61535) SHA1(75b9a805f8639554251192e3777073c29952c78f))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("sound_m1.bin", 0x0000, 0x2000, CRC(21a9ee1d) SHA1(d906ac7d6e741f05e81076a5be33fc763f0de9c1))

	ROM_REGION(0xa0000, "musicrom", ROMREGION_ERASEFF)
	ROM_LOAD("sound_m2.bin", 0x40000, 0x20000, CRC(349df1fe) SHA1(47e7ddbdc398396e40bb5340e5edcb8baf06c255))
	ROM_LOAD("sound_m3.bin", 0x80000, 0x20000, CRC(15ef1866) SHA1(4ffa3b29bf3c30a9a5bc622adde16a1a13833b22))
ROM_END

/*-------------------------------------------------------------------
/ Bushido (1993)
/-------------------------------------------------------------------*/
ROM_START(bushido)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("0-z80.bin", 0x0000, 0x2000, CRC(3ea1eb1d) SHA1(cceb6c68e481f36a5646ff4f38d3dfc4275b0c79))
	ROM_LOAD("1-z80.old", 0x2000, 0x2000, CRC(648da72b) SHA1(1005a13b4746e302d979c8b1da300e943cdcab3d))

	ROM_REGION(0x10000, "dmdcpu", 0)
	ROM_LOAD("g-disply.bin", 0x00000, 0x10000, CRC(9a1df82f) SHA1(4ad6a12ae36ec898b8ac5243da6dec3abcd9dc33))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("a-sonido.bin", 0x0000, 0x2000, CRC(cf7d5399) SHA1(c79145826cfa6be2487e3add477d9b452c553762))

	ROM_REGION(0x100000, "audiorom", 0)
	ROM_LOAD("b-sonido.bin", 0x00000, 0x80000, CRC(cb4fc885) SHA1(569f389fa8f91f886b58f44f701d2752ef01f3fa))
	ROM_LOAD("c-sonido.bin", 0x80000, 0x80000, CRC(35a43dd8) SHA1(f2b1994f67f749c65a88c95d970b655990d85b96))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("d-musica.bin", 0x0000, 0x2000, CRC(2cb9697c) SHA1(d5c66d616ccd5e299832704e494743429dafd569))

	ROM_REGION(0x100000, "musicrom", 0)
	ROM_LOAD("e-musica.bin", 0x00000, 0x80000, CRC(1414b921) SHA1(5df9e538ee109df28953ec8f162c60cb8c6e4d96))
	ROM_LOAD("f-musica.bin", 0x80000, 0x80000, CRC(80f3a6df) SHA1(e09ad4660e511779c6e55559fa0c2c0b0c6600c8))
ROM_END

ROM_START(bushidoa)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("0-cpu.bin", 0x0000, 0x2000, CRC(7f7e6642) SHA1(6872397eed7525f384b79cdea13531d273d8cf14))
	ROM_LOAD("1-cpu.bin", 0x2000, 0x2000, CRC(a538d37f) SHA1(d2878ad0d31b4221b823812485c7faaf666ce185))

	ROM_REGION(0x10000, "dmdcpu", 0)
	ROM_LOAD("g-disply.bin", 0x00000, 0x10000, CRC(9a1df82f) SHA1(4ad6a12ae36ec898b8ac5243da6dec3abcd9dc33))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("a-sonido.bin", 0x0000, 0x2000, CRC(cf7d5399) SHA1(c79145826cfa6be2487e3add477d9b452c553762))

	ROM_REGION(0x100000, "audiorom", 0)
	ROM_LOAD("b-sonido.bin", 0x00000, 0x80000, CRC(cb4fc885) SHA1(569f389fa8f91f886b58f44f701d2752ef01f3fa))
	ROM_LOAD("c-sonido.bin", 0x80000, 0x80000, CRC(35a43dd8) SHA1(f2b1994f67f749c65a88c95d970b655990d85b96))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("d-musica.bin", 0x0000, 0x2000, CRC(2cb9697c) SHA1(d5c66d616ccd5e299832704e494743429dafd569))

	ROM_REGION(0x100000, "musicrom", 0)
	ROM_LOAD("e-musica.bin", 0x00000, 0x80000, CRC(1414b921) SHA1(5df9e538ee109df28953ec8f162c60cb8c6e4d96))
	ROM_LOAD("f-musica.bin", 0x80000, 0x80000, CRC(80f3a6df) SHA1(e09ad4660e511779c6e55559fa0c2c0b0c6600c8))
ROM_END

ROM_START(bushidob)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("inder_sa_m-17_pb-in_a-1954-93_b4_cpu rom_0_06-a.bin", 0x0000, 0x2000, CRC(7c8561f7) SHA1(d81338160f6eb8898a7b779f57e327d2db3f7dab))
	ROM_LOAD("inder_sa_m-17_pb-in_a-1954-93_b4_cpu rom_1_06-a.bin", 0x2000, 0x2000, CRC(4a41865c) SHA1(f597d5dd42b804ab637408b90d05bdb234559a48))

	ROM_REGION(0x10000, "dmdcpu", 0)
	ROM_LOAD("g-disply.bin", 0x00000, 0x10000, CRC(9a1df82f) SHA1(4ad6a12ae36ec898b8ac5243da6dec3abcd9dc33))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("a-sonido.bin", 0x0000, 0x2000, CRC(cf7d5399) SHA1(c79145826cfa6be2487e3add477d9b452c553762))

	ROM_REGION(0x100000, "audiorom", 0)
	ROM_LOAD("b-sonido.bin", 0x00000, 0x80000, CRC(cb4fc885) SHA1(569f389fa8f91f886b58f44f701d2752ef01f3fa))
	ROM_LOAD("c-sonido.bin", 0x80000, 0x80000, CRC(35a43dd8) SHA1(f2b1994f67f749c65a88c95d970b655990d85b96))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("d-musica.bin", 0x0000, 0x2000, CRC(2cb9697c) SHA1(d5c66d616ccd5e299832704e494743429dafd569))

	ROM_REGION(0x100000, "musicrom", 0)
	ROM_LOAD("e-musica.bin", 0x00000, 0x80000, CRC(1414b921) SHA1(5df9e538ee109df28953ec8f162c60cb8c6e4d96))
	ROM_LOAD("f-musica.bin", 0x80000, 0x80000, CRC(80f3a6df) SHA1(e09ad4660e511779c6e55559fa0c2c0b0c6600c8))
ROM_END

/*-------------------------------------------------------------------
/ Mach 2 (1995)
/-------------------------------------------------------------------*/
ROM_START(mach2)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("m2cpu0.19", 0x0000, 0x2000, CRC(274c8040) SHA1(6b039b79b7e08f2bf2045bc4f1cbba790c999fed))
	ROM_LOAD("m2cpu1.19", 0x2000, 0x2000, CRC(c445df0b) SHA1(1f346c1df8df0a3c4e8cb1186280d2f34959b3f8))

	ROM_REGION(0x10000, "dmdcpu", 0)
	ROM_LOAD("m2dmdf.01", 0x00000, 0x10000, CRC(c45ccc74) SHA1(8362e799a76536a16dd2d5dde500ad3db273180f))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("m2sndd.01", 0x0000, 0x2000, CRC(e789f22d) SHA1(36aa7eac1dd37a02c982d109462dddbd85a305cc))

	ROM_REGION(0x80000, "audiorom", 0)
	ROM_LOAD("m2snde.01", 0x00000, 0x80000, CRC(f5721119) SHA1(9082198e8d875b67323266c4bf8c2c378b63dfbb))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("m2musa.01", 0x0000, 0x2000, CRC(2d92a882) SHA1(cead22e434445e5c25414646b1e9ae2b9457439d))

	ROM_REGION(0x100000, "musicrom", 0)
	ROM_LOAD("m2musb.01", 0x00000, 0x80000, CRC(6689cd19) SHA1(430092d51704dfda8bd8264875f1c1f4461c56e5))
	ROM_LOAD("m2musc.01", 0x80000, 0x80000, CRC(88851b82) SHA1(d0c9fa391ca213a69b7c8ae7ca52063503b5656e))
ROM_END

ROM_START(mach2a)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("mach_cpu_0.bin", 0x0000, 0x2000, CRC(4eee3d63) SHA1(dc7c919dd909a134ec434ce665c421aa03aaa637))
	ROM_LOAD("mach_cpu_1.bin", 0x2000, 0x2000, CRC(c115b900) SHA1(712ae2a38528c05f28e3743cb49e072fc05bf908))

	ROM_REGION(0x10000, "dmdcpu", 0)
	ROM_LOAD("m2dmdf.01", 0x00000, 0x10000, CRC(c45ccc74) SHA1(8362e799a76536a16dd2d5dde500ad3db273180f))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("m2sndd.01", 0x0000, 0x2000, CRC(e789f22d) SHA1(36aa7eac1dd37a02c982d109462dddbd85a305cc))

	ROM_REGION(0x80000, "audiorom", 0)
	ROM_LOAD("m2snde.01", 0x00000, 0x80000, CRC(f5721119) SHA1(9082198e8d875b67323266c4bf8c2c378b63dfbb))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("m2musa.01", 0x0000, 0x2000, CRC(2d92a882) SHA1(cead22e434445e5c25414646b1e9ae2b9457439d))

	ROM_REGION(0x100000, "musicrom", 0)
	ROM_LOAD("m2musb.01", 0x00000, 0x80000, CRC(6689cd19) SHA1(430092d51704dfda8bd8264875f1c1f4461c56e5))
	ROM_LOAD("m2musc.01", 0x80000, 0x80000, CRC(88851b82) SHA1(d0c9fa391ca213a69b7c8ae7ca52063503b5656e))
ROM_END

/*-------------------------------------------------------------------
/ Jolly Park (1996)
/-------------------------------------------------------------------*/
ROM_START(jolypark)
	ROM_REGION(0x4000, "maincpu", 0)
	ROM_LOAD("jpcpu0.rom", 0x0000, 0x2000, CRC(061967af) SHA1(45048e1d9f17efa3382460fd474a5aeb4191d617))
	ROM_LOAD("jpcpu1.rom", 0x2000, 0x2000, CRC(ea99202f) SHA1(e04825e73fd25f6469b3315f063f598ea1ab44c7))

	ROM_REGION(0x30000, "dmdcpu", 0)
	ROM_LOAD("jpdmd0.rom", 0x00000, 0x10000, CRC(b57565cb) SHA1(3fef66d298893029de78fdb6ecdb562c33d76180))
	ROM_LOAD("jpdmd1.rom", 0x10000, 0x20000, CRC(40d1563f) SHA1(90dbea742202340da6fa950eedc2bceec5a2af7e))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("jpsndc1.rom", 0x0000, 0x2000, CRC(0475318f) SHA1(7154bd5ca5b28019eb0ff598ec99bbe49260932b))

	ROM_REGION(0x100000, "audiorom", 0)
	ROM_LOAD("jpsndm4.rom", 0x00000, 0x80000, CRC(735f3db7) SHA1(81dc893f5194d6ac1af54b262555a40c5c3e0292))
	ROM_LOAD("jpsndm5.rom", 0x80000, 0x80000, CRC(769374bd) SHA1(8121369714c55cc06c493b15e5c2ca79b13aff52))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("jpsndc0.rom", 0x0000, 0x2000, CRC(a97259dc) SHA1(58dea3f36b760112cfc32d306077da8cf6cdec5a))

	ROM_REGION(0x180000, "musicrom", 0)
	ROM_LOAD("jpsndm1.rom", 0x000000, 0x80000, CRC(fc91d2f1) SHA1(c838a0b31bbec9dbc96b46d692c8d6f1286fe46a))
	ROM_LOAD("jpsndm2.rom", 0x080000, 0x80000, CRC(fb2d1882) SHA1(fb0ef9def54d9163a46354a0df0757fac6cbd57c))
	ROM_LOAD("jpsndm3.rom", 0x100000, 0x80000, CRC(77e515ba) SHA1(17b635d107c437bfc809f8cc1a6cd063cef12691))
ROM_END

/*-------------------------------------------------------------------
/ Verne's World (1996)
/-------------------------------------------------------------------*/
ROM_START(vrnwrld)
	ROM_REGION(0x8000, "maincpu", 0)
	ROM_LOAD("vwcpu0.rom", 0x0000, 0x4000, CRC(89c980e7) SHA1(09eeed0232255875cf119d59143d839ff40e30dd))
	ROM_LOAD("vwcpu1.rom", 0x4000, 0x4000, CRC(a4db4e64) SHA1(fc55781295fc723741de24ad60311b7e33551830))

	ROM_REGION(0x110000, "dmdcpu", 0)
	ROM_LOAD("vwdmd0.rom", 0x00000, 0x10000, CRC(40600060) SHA1(7ad619bcb5e5e50325360f4e946b5bfa072caead))
	ROM_LOAD("vwdmd1.rom", 0x10000, 0x80000, CRC(de4a1060) SHA1(6b848dfd8aafdbcf7e1593f98bd1c3d69306aa11))
	ROM_LOAD("vwdmd2.rom", 0x90000, 0x80000, CRC(29fc8da7) SHA1(2704f14a3338a63abda3bcbc56e9f984a679eb38))

	ROM_REGION(0x2000, "audiocpu", 0)
	ROM_LOAD("vws2ic9.rom", 0x0000, 0x2000, CRC(ab8cb4c5) SHA1(92a702c11e2cef703992244529ba86079d5ab9b0))

	ROM_REGION(0x80000, "audiorom", 0)
	ROM_LOAD("vws3ic15.rom", 0x00000, 0x80000, CRC(d62c9443) SHA1(7c6b8662d88ba6592da8b83af11087647105e8dd))

	ROM_REGION(0x2000, "musiccpu", 0)
	ROM_LOAD("vws4ic30.rom", 0x0000, 0x2000, CRC(ecd18a19) SHA1(558e687e0429d31fafe8db05954d9a8ad90d6aeb))

	ROM_REGION(0x180000, "musicrom", 0)
	ROM_LOAD("vws5ic25.rom", 0x000000, 0x80000, CRC(56d349f0) SHA1(e71d2d03c3e978c552e272de8850cc265255fbd1))
	ROM_LOAD("vws6ic26.rom", 0x080000, 0x80000, CRC(bee399c1) SHA1(b2c6e4830641ed32b9643dc8c1fa08a2da5a7e9b))
	ROM_LOAD("vws7ic27.rom", 0x100000, 0x80000, CRC(7335b29c) SHA1(4de6de09f069feecbad2e5ef50032e8d381ff9b1))
ROM_END

} // Anonymous namespace

GAME(1992, metalman, 0,       metalman, metalman, spinb_state, init_3, ROT0, "Inder",    "Metal Man",       MACHINE_IS_SKELETON_MECHANICAL )
GAME(1993, bushido,  0,       spinb,    bushido,  spinb_state, init_0, ROT0, "Inder",    "Bushido (set 1)", MACHINE_IS_SKELETON_MECHANICAL )
GAME(1993, bushidoa, bushido, spinb,    bushido,  spinb_state, init_0, ROT0, "Inder",    "Bushido (set 2)", MACHINE_IS_SKELETON_MECHANICAL )
GAME(1993, bushidob, bushido, spinb,    bushido,  spinb_state, init_0, ROT0, "Inder",    "Bushido (set 3)", MACHINE_IS_SKELETON_MECHANICAL )
GAME(1995, mach2,    0,       spinb,    mach2,    spinb_state, init_0, ROT0, "Spinball", "Mach 2 (set 1)",  MACHINE_IS_SKELETON_MECHANICAL )
GAME(1995, mach2a,   mach2,   spinb,    mach2,    spinb_state, init_0, ROT0, "Spinball", "Mach 2 (set 2)",  MACHINE_IS_SKELETON_MECHANICAL )
GAME(1996, jolypark, 0,       jolypark, spinb,    spinb_state, init_1, ROT0, "Spinball", "Jolly Park",      MACHINE_IS_SKELETON_MECHANICAL )
GAME(1996, vrnwrld,  0,       vrnwrld,  vrnwrld,  spinb_state, init_2, ROT0, "Spinball", "Verne's World",   MACHINE_IS_SKELETON_MECHANICAL )

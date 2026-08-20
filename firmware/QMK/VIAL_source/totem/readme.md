# TOTEM 20 — variante sem encoder e sem display

Macropad unibody de **20 teclas**. Sem encoder, sem OLED: tudo é controlado
pelas próprias teclas, por meio de dois acordes de duas teclas.

Fork do [TOTEM](https://github.com/GEIGEIGEIST/TOTEM), o split de 38 teclas do
@geigeigeist, reconstruído como peça única. **Não é split**: não há segunda
metade, link serial, handedness nem `EE_HANDS`.

> Esta é a variante enxuta. A versão com encoder e display está na branch
> `encoder-oled-screen`.

## Hardware

| | |
|---|---|
| MCU | Waveshare RP2040-Zero **ou** Seeed XIAO RP2040 |
| Matriz | 4 linhas × 6 colunas — 20 teclas |
| Diodos | um por tecla, `COL2ROW` |
| RGB | 20 LEDs endereçáveis, um por tecla, dados em `GP26`, 5 V do VBUS |

### O XIAO RP2040 volta a servir

Sem encoder e sem display, o orçamento de pinos cai para **11**:

```
6 colunas + 4 linhas + 1 dado de RGB = 11
```

Que é exatamente o que o XIAO RP2040 expõe. A variante com encoder e OLED
precisava de 15 e por isso exigia o RP2040-Zero; esta cabe nos dois.

**Pinagem padrão — RP2040-Zero:**

| Sinal | GPIO |
|---|---|
| COL1..COL6 | `GP0` `GP1` `GP2` `GP3` `GP4` `GP5` |
| ROW1..ROW4 | `GP6` `GP7` `GP8` `GP9` |
| RGB data | `GP26` |

**Alternativa — XIAO RP2040.** Troque o bloco `matrix_pins` e o `ws2812.pin`
do `keyboard.json` por:

```json
"matrix_pins": {
    "rows": ["GP1", "GP2", "GP4", "GP3"],
    "cols": ["GP7", "GP6", "GP29", "GP28", "GP27", "GP26"]
},
"ws2812": { "pin": "GP0", "driver": "vendor" }
```

Correspondência de pads do XIAO: COL1..COL6 = `D5 D4 D3 D2 D1 D0`,
ROW1..ROW4 = `D10 D9 D8 D7`, dados dos LEDs = `D6`. Os 11 pads ficam ocupados,
sem sobra.

### Convenção de índices da matriz

O firmware indexa as linhas de cima para baixo, o **inverso** dos nomes do
esquema:

| Firmware | Esquema | GPIO (Zero) |
|---|---|---|
| row 0 | ROW4 | `GP9` |
| row 1 | ROW3 | `GP8` |
| row 2 | ROW2 | `GP7` |
| row 3 | ROW1 | `GP6` |
| col 0..5 | COL1..COL6 | `GP0`..`GP5` |

### Mapa dos switches

| SW | Linha | Coluna | Pos. `LAYOUT` | Keycode no jogo |
|---|---|---|---|---|
| SW1  | 0 (ROW4) | 1 (COL2) | 1  | `KC_1` |
| SW2  | 0 (ROW4) | 2 (COL3) | 2  | `KC_2` |
| SW3  | 0 (ROW4) | 3 (COL4) | 3  | `KC_3` |
| SW4  | 0 (ROW4) | 4 (COL5) | 4  | `KC_R` |
| SW5  | 1 (ROW3) | 0 (COL1) | 5  | `KC_Q` |
| SW6  | 1 (ROW3) | 1 (COL2) | 6  | `KC_A` |
| SW7  | 1 (ROW3) | 2 (COL3) | 7  | `KC_W` |
| SW8  | 1 (ROW3) | 3 (COL4) | 8  | `KC_D` |
| SW9  | 1 (ROW3) | 4 (COL5) | 9  | `KC_F` |
| SW10 | 2 (ROW2) | 0 (COL1) | 10 | `KC_LSFT` |
| SW11 | 2 (ROW2) | 1 (COL2) | 11 | `KC_Z` |
| SW12 | 2 (ROW2) | 2 (COL3) | 12 | `KC_S` |
| SW13 | 2 (ROW2) | 3 (COL4) | 13 | `KC_X` |
| SW14 | 2 (ROW2) | 4 (COL5) | 14 | `KC_T` |
| SW15 | 2 (ROW2) | 5 (COL6) | 15 | `KC_E` |
| SW16 | 3 (ROW1) | 0 (COL1) | 16 | `KC_G` |
| SW17 | 3 (ROW1) | 1 (COL2) | 17 | `KC_LCTL` |
| SW18 | 3 (ROW1) | 3 (COL4) | 18 | `KC_C` |
| SW19 | 3 (ROW1) | 4 (COL5) | 19 | `KC_SPC` |
| SW20 | 3 (ROW1) | 5 (COL6) | 20 | `KC_B` |

`[0,0]`, `[0,5]`, `[1,5]` e `[3,2]` não têm switch e são `KC_NO`.

## Os dois acordes

Ambos ancorados no **G**, para haver uma única tecla a lembrar:

| Acorde | Segurar | Faz |
|---|---|---|
| **G + B** | 1 s | Próximo perfil: Jogo → 3ds Max → Macros → Jogo |
| **G + E** | 1 s | Liga/desliga a camada de RGB |

G, B e E são os cantos mais distantes do teclado — G embaixo à esquerda, B e E
na ponta do cluster do polegar. Acionamento simultâneo acidental durante o jogo
é praticamente impossível.

Regras aplicadas pelo motor de acordes em `totem.c`:

- as duas teclas devem descer dentro de `CHORD_SYNC_MS` (50 ms) uma da outra —
  apertar G, esperar e depois apertar B nunca arma nada;
- o acorde precisa ser mantido por `CHORD_HOLD_MS` (1000 ms);
- enquanto armado, **nenhum dos keycodes chega ao computador**, então uma troca
  reconhecida nunca vaza um `G` ou `B` perdido no jogo;
- se soltar antes, as duas teclas são entregues normalmente — nada se perde;
- pressionadas sozinhas, G, B e E funcionam como teclas comuns.

Todas as coordenadas e tempos estão em `config.h`. Para mover um acorde, mude
só as coordenadas.

> **Ressalva:** as teclas dos acordes ficam retidas por até 50 ms e são
> reproduzidas com `register_code16()`. Mantenha-as em keycodes *básicos* —
> layer taps, tap dance e macros do Vial nessas três posições não reproduzem
> corretamente.

## Camada de RGB

Entre com **G + E** por 1 segundo. O mesmo acorde sai. No efeito sólido a placa
fica amarela ao entrar, para você saber que a camada está ativa.

```
            efeito-  efeito+  liga/desl   BOOT
   brilho-   brilho+  matiz-   matiz+     satur+
   satur-    veloc-   veloc+   EE_CLR     debug     [E]
   [G]       gravar            tocar      parar     [B]
```

| Tecla | Função |
|---|---|
| `1` / `2` | Efeito anterior / próximo |
| `3` | Liga e desliga os LEDs |
| `R` | Bootloader |
| `Q` / `A` | Brilho − / + |
| `W` / `D` | Matiz − / + |
| `F` / `Shift` | Saturação + / − |
| `Z` / `S` | Velocidade da animação − / + |
| `X` | Limpar EEPROM |
| `T` | Alternar debug |
| `Ctrl` / `C` / `Espaço` | Gravar / tocar / parar macro |

G, B e E ficam mortas nessa camada de propósito: existem ali só como parceiras
de acorde, então um toque solto não faz nada.

### Efeitos disponíveis

`solid_color` · `breathing` (pulsar) · `hue_breathing` (pulsar de cor) ·
`cycle_all` · `cycle_left_right` · `cycle_up_down` · `rainbow_moving_chevron` ·
`jellybean_raindrops` · `pixel_rain` (piscar aleatório) · `typing_heatmap`
(mapa de calor do que você digita) · `digital_rain` · `solid_reactive_simple` ·
`solid_reactive` · `splash` · `solid_splash`

Os quatro últimos reagem ao toque das teclas.

## Perfis

Cinco camadas configuráveis no Vial, com as 20 teclas livres em cada uma:

| # | Perfil | Cor |
|---|---|---|
| 0 | Jogo | vermelho |
| 1 | 3ds Max | azul |
| 2 | Macros / uso geral | verde |
| 3 | Reservada | roxo |
| 4 | RGB / manutenção | amarelo |

O perfil escolhido é gravado na EEPROM, então **sobrevive a reinício**.

A camada 3 fica fora do ciclo do acorde — alcance-a atribuindo `MO()`, `TG()`
ou `TO()` a qualquer tecla no Vial.

> **Como a cor do perfil convive com os seus ajustes:** a indicação de perfil
> mexe **apenas na matiz, e apenas no efeito sólido**. Em qualquer animação o
> firmware não toca na cor — senão toda troca de camada atropelaria a paleta
> que você acabou de definir. Brilho e efeito nunca são alterados.

## RGB — ligação

**A cadeia precisa ser roteada DIN → DOUT seguindo SW1 até SW20**, na mesma
ordem da tabela de switches. O `RGB_MATRIX` precisa saber qual LED corresponde
a qual tecla; esse mapa é o `g_led_config` em `totem.c`. Se o PCB serpentear
por linha, muda só o primeiro bloco daquela struct.

- **330 Ω** em série na linha de dados, junto ao primeiro LED.
- **100 nF** por LED, entre VCC e GND.
- **470–1000 µF** na entrada do barramento de 5 V.
- `max_brightness` travado em **120**: 20 WS2812 em branco pleno passam de 1 A,
  muito além dos 500 mA do USB. Uso real — cor sólida com brilho 80 — fica em
  120–150 mA.

> **Nível lógico:** `GP26` entrega 3,3 V, mas um WS2812B em 5 V espera
> 0,7 × VDD = 3,5 V. Costuma funcionar, mas se o primeiro LED se comportar mal:
> use SK6812, baixe a alimentação para ~4,3 V com um diodo em série, ou use um
> 74AHCT125.

## Compilar

Este repositório é um projeto de hardware, não uma árvore QMK — não há build
system dentro dele. A pasta `totem/` é só a *definição* do teclado e precisa
estar dentro de uma árvore QMK. É obrigatório o fork **Vial**.

```sh
git clone --recurse-submodules https://github.com/vial-kb/vial-qmk.git ~/vial-qmk
ln -s "$(pwd)/firmware/QMK/VIAL_source/totem" ~/vial-qmk/keyboards/totem
```

Symlink, não cópia: um único conjunto de arquivos, sem duas versões divergindo
em silêncio.

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk

make totem:vial       # -> totem_vial.uf2    119 KiB · recomendado
make totem:default    # -> totem_default.uf2  83 KiB · QMK puro
```

Três armadilhas de ambiente:

- **Python precisa ser 3.11.** Os scripts do QMK usam `ast.Num`, removido no 3.12.
- O **`arm-none-eabi-gcc` do Homebrew não serve** — vem sem newlib e o build
  morre em `fatal error: stdint.h`. Use o toolchain oficial da ARM.
- `brew install qmk/qmk/qmk` pode sair com código 0 **sem instalar nada**,
  bloqueado pela política de tap trust.

## Gravar

1. Duplo toque no RESET. A placa monta como pendrive `RPI-RP2`.
2. Copie o `.uf2` para dentro.
3. Reinicia sozinha e o pendrive some.

Se o `RPI-RP2` não aparecer: segure **BOOT**, pressione e solte **RESET**,
solte **BOOT**.

## Vial

`VIAL_KEYBOARD_UID` é própria e diferente da do TOTEM original — o Vial indexa
definições por UID, e reaproveitar faria carregar o layout errado de 38 teclas.

Combo de destravamento: **`1` + `R`** (`[0,1]` e `[0,4]`).

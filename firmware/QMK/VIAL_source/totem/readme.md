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
| MCU | **Seeed XIAO RP2040** (ou Waveshare RP2040-Zero) |
| Matriz | 4 linhas × 6 colunas — 20 teclas |
| Diodos | um por tecla, `COL2ROW` |
| RGB | 20 LEDs endereçáveis, um por tecla, dados em `GP0` (pad `D6`), 5 V do VBUS |

### Por que o XIAO RP2040

Sem encoder e sem display, o orçamento de pinos cai para **11**:

```
6 colunas + 4 linhas + 1 dado de RGB = 11
```

Que é exatamente o que o XIAO RP2040 expõe — e é exatamente a pinagem do
esquema elétrico do projeto, pino a pino, incluindo o `LEDS_SIGNAL` no `D6`.
A variante com encoder e OLED precisa de 15 pinos e por isso exige o
RP2040-Zero; esta cabe no XIAO, sem sobra.

**Pinagem — XIAO RP2040:**

| Sinal | GPIO | Pad | Pino |
|---|---|---|---|
| COL1 | `GP7` | `D5` | 6 |
| COL2 | `GP6` | `D4` | 5 |
| COL3 | `GP29` | `D3` | 4 |
| COL4 | `GP28` | `D2` | 3 |
| COL5 | `GP27` | `D1` | 2 |
| COL6 | `GP26` | `D0` | 1 |
| ROW1 | `GP3` | `D10` | 11 |
| ROW2 | `GP4` | `D9` | 10 |
| ROW3 | `GP2` | `D8` | 9 |
| ROW4 | `GP1` | `D7` | 8 |
| RGB data | `GP0` | `D6` | 7 |

Os 11 pads ficam ocupados, sem nenhum sobrando.

**Alternativa — Waveshare RP2040-Zero.** Se preferir usar a mesma placa da
branch `encoder-oled-screen`, troque `matrix_pins` e `ws2812.pin` no
`keyboard.json` por:

```json
"matrix_pins": {
    "rows": ["GP9", "GP8", "GP7", "GP6"],
    "cols": ["GP0", "GP1", "GP2", "GP3", "GP4", "GP5"]
},
"ws2812": { "pin": "GP26", "driver": "vendor" }
```

### Convenção de índices da matriz

O firmware indexa as linhas de cima para baixo, o **inverso** dos nomes do
esquema:

| Firmware | Esquema | GPIO | Pad |
|---|---|---|---|
| row 0 | ROW4 | `GP1` | `D7` |
| row 1 | ROW3 | `GP2` | `D8` |
| row 2 | ROW2 | `GP4` | `D9` |
| row 3 | ROW1 | `GP3` | `D10` |
| col 0..5 | COL1..COL6 | `GP7` `GP6` `GP29` `GP28` `GP27` `GP26` | `D5`..`D0` |

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

## Materiais e ligação completa

| Qtd | Item | Observação |
|---|---|---|
| 1 | Seeed XIAO RP2040 | Ou Waveshare RP2040-Zero — os dois servem nesta variante |
| 20 | Switches mecânicos | MX ou Choc, conforme a carcaça |
| 20 | Diodos 1N4148 | Vidro, ou 1N4148W em SOD-123 para SMD |
| 20 | LEDs SK6812MINI-E | Preferível ao WS2812B — ver nota de nível lógico |
| 1 | Resistor 330 Ω | Em série na linha de dados |
| 20 | Capacitor 100 nF cerâmico | Um por LED |
| 1 | Capacitor 470–1000 µF | Eletrolítico, na entrada dos 5 V |
| 0–1 | 74AHCT125 | Só se o nível lógico de 3,3 V der problema |

### Onde cada pino termina

Os 11 GPIOs em uso, mais alimentação. Se um fio não estiver nesta tabela, ele
não deveria existir.

| Pad | GPIO | Recebe | Quantos fios |
|---|---|---|---|
| `D5` | `GP7` | COL1 — um terminal de Q, Shift, G | 3 switches |
| `D4` | `GP6` | COL2 — 1, A, Z, Ctrl | 4 switches |
| `D3` | `GP29` | COL3 — 2, W, S | 3 switches |
| `D2` | `GP28` | COL4 — 3, D, X, C | 4 switches |
| `D1` | `GP27` | COL5 — R, F, T, Espaço | 4 switches |
| `D0` | `GP26` | COL6 — E, B | 2 switches |
| `D10` | `GP3` | ROW1 — cátodos de G, Ctrl, C, Espaço, B | 5 diodos |
| `D9` | `GP4` | ROW2 — cátodos de Shift, Z, S, X, T, E | 6 diodos |
| `D8` | `GP2` | ROW3 — cátodos de Q, A, W, D, F | 5 diodos |
| `D7` | `GP1` | ROW4 — cátodos de 1, 2, 3, R | 4 diodos |
| `D6` | `GP0` | DIN do LED 1, via 330 Ω | 1 |
| `5V` | VBUS | VCC de todos os 20 LEDs | barramento |
| `GND` | — | GND de todos os 20 LEDs | barramento |

Conferência rápida: 3+4+3+4+4+2 = 20 switches nas colunas, e 5+6+5+4 = 20
diodos nas linhas. Contagem diferente significa fio faltando ou sobrando.

### Ordem de montagem

1. **Colunas primeiro** — seis fios, cada um passando por seus switches.
2. **Diodo em cada switch** — ânodo no switch, cátodo (faixa) para a linha.
3. **Linhas** — quatro fios recolhendo os cátodos.
4. **Teste a matriz inteira** antes de encostar nos LEDs. É muito mais fácil
   corrigir um diodo sem a cadeia de LEDs por cima.
5. **LEDs por último**, em cadeia, começando com dois ou três.

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

> **Nível lógico:** `GP0` entrega 3,3 V, mas um WS2812B em 5 V espera
> 0,7 × VDD = 3,5 V. Costuma funcionar, mas se o primeiro LED se comportar mal:
> use SK6812, baixe a alimentação para ~4,3 V com um diodo em série, ou use um
> 74AHCT125.

## Compilar o firmware do zero

Do nada instalado até um `.uf2`. Comandos verificados em macOS/arm64, agosto de
2026.

Este repositório é um **projeto de hardware**, não uma árvore QMK: não há build
system dentro dele. A pasta `totem/` é só a *definição* do teclado e precisa
estar dentro de uma árvore QMK que traga o `quantum/`, o ChibiOS, o pico-sdk e
os Makefiles — cerca de 1,6 GB, deliberadamente não versionado aqui.

É obrigatório o fork **Vial** do QMK. O código Vial **não compila** no
`qmk_firmware` upstream.

### 1. Python 3.11

Não é preciosismo: os scripts do QMK usam `ast.Num`, removido no Python 3.12.
Com 3.12 ou mais novo o build morre em
`AttributeError: module 'ast' has no attribute 'Num'`.

```sh
brew install python@3.11
```

### 2. Clonar o vial-qmk

```sh
git clone --recurse-submodules https://github.com/vial-kb/vial-qmk.git ~/vial-qmk
```

O `--recurse-submodules` é obrigatório: sem ele faltam o ChibiOS e o pico-sdk,
e o build falha lá na frente sem dizer o porquê.

### 3. Ambiente Python

```sh
/opt/homebrew/opt/python@3.11/bin/python3.11 -m venv ~/vial-qmk/.venv
~/vial-qmk/.venv/bin/pip install --upgrade pip
~/vial-qmk/.venv/bin/pip install -r ~/vial-qmk/requirements.txt qmk
```

### 4. Toolchain ARM

Precisa incluir a **newlib**. O `arm-none-eabi-gcc` do homebrew-core **não
serve** — vem sem os headers de libc e o build morre em
`fatal error: stdint.h: No such file or directory`.

**Caminho A — cask oficial** (pede senha de sudo):

```sh
brew install --cask gcc-arm-embedded
```

**Caminho B — sem sudo**, extraindo o mesmo `.pkg`. Foi assim que este firmware
foi compilado:

```sh
brew fetch --cask gcc-arm-embedded
PKG=$(find ~/Library/Caches/Homebrew/downloads -name "*arm-gnu-toolchain*.pkg" | head -1)

mkdir -p /tmp/armx && cd /tmp/armx
xar -xf "$PKG"
mkdir -p out && cat Payload | gzip -dc | cpio -idm -D out

mv out ~/vial-qmk/.toolchain
~/vial-qmk/.toolchain/bin/arm-none-eabi-gcc --version   # confere
```

### 5. Ligar a definição do teclado

```sh
cd <este-repositorio>
git checkout without-encoder-oled-screen
ln -s "$(pwd)/firmware/QMK/VIAL_source/totem" ~/vial-qmk/keyboards/totem
```

Symlink, **nunca cópia**. Copiar cria duas versões dos mesmos arquivos: você
edita no repositório, esquece de copiar, compila a versão velha e não entende
por que a mudança não surtiu efeito.

### 6. Compilar

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk

make totem:vial       # -> totem_vial.uf2    119 KiB · recomendado
make totem:default    # -> totem_default.uf2  83 KiB · QMK puro
```

O `export PATH` precisa vir **antes** do `make`, e em toda sessão de terminal
nova. Um build limpo termina em `Creating UF2 file for deployment` e `[OK]`.
Qualquer `[ERRORS]` é erro real — o QMK trata warnings como erro, então não
existe "compilou com avisos".

> **Armadilha:** `brew install qmk/qmk/qmk` pode terminar com **código de saída
> 0 sem instalar nada**, bloqueado pela política de tap trust do Homebrew — a
> saída diz `Would install 1 formula` e para por aí. Os passos acima evitam o
> problema por não dependerem daquele tap.

### Recompilar depois de uma alteração

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk && make totem:vial
```

Os passos 1 a 5 são uma vez só. Se algo ficar estranho depois de mudar
`keyboard.json` ou `rules.mk`, apague o cache com `rm -rf ~/vial-qmk/.build`.

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

# Allien — variante com encoder e display

Macropad unibody de **20 teclas** com encoder rotativo e display OLED SSD1306,
sobre um Waveshare RP2040-Zero.

Fork do [TOTEM](https://github.com/GEIGEIGEIST/TOTEM), o split de 38 teclas com
column stagger do @geigeigeist, reconstruído como peça única. **Não é split**:
não há segunda metade, link serial, handedness nem `EE_HANDS`.

> Esta é a variante completa. A versão enxuta, sem encoder e sem display, está
> na branch `without-encoder-oled-screen`.

## Hardware

| | |
|---|---|
| MCU | Waveshare RP2040-Zero |
| Matriz | 4 linhas × 6 colunas — 20 teclas + botão do encoder = 21 switches |
| Diodos | um por switch, `COL2ROW` |
| RGB | 20 LEDs endereçáveis, um por tecla, dados em `GP26`, 5 V do VBUS |
| Encoder | EC11, `GP14`/`GP15`; o botão dele é uma tecla da matriz |
| Display | SSD1306 128×64 I²C, `GP10` (SDA) / `GP11` (SCL) |

### Por que não o XIAO RP2040

O TOTEM original usava um Seeed XIAO RP2040, que expõe **11 GPIOs**. Esta
variante precisa de **15**:

```
6 colunas + 4 linhas + 1 RGB + 2 encoder + 2 I²C = 15
```

Nenhum arranjo cabe: encoder e OLED sem RGB dá 14; só o encoder, sem OLED e sem
RGB, ainda dá 12. A matriz também não pode encolher — 21 switches exigem no
mínimo 10 pinos (4×6, 5×5 e 3×7 custam todos 10). O RP2040-Zero expõe 20 GPIOs
na borda castellada, então os 15 cabem com folga de 5.

O **botão do encoder não custa GPIO nenhum**: ele é ligado na matriz, na posição
`[0,5]`, uma das vagas que a grade 4×6 deixa livres.

### Pinagem

| Sinal | GPIO | Observação |
|---|---|---|
| COL1..COL6 | `GP0` `GP1` `GP2` `GP3` `GP4` `GP5` | |
| ROW1..ROW4 | `GP6` `GP7` `GP8` `GP9` | |
| OLED SDA | `GP10` | I²C1 |
| OLED SCL | `GP11` | I²C1 |
| Encoder A | `GP14` | |
| Encoder B | `GP15` | |
| Dados RGB | `GP26` | cadeia WS2812 |
| livres | `GP12` `GP13` `GP27` `GP28` `GP29` | |

`GP16` fica deliberadamente sem uso — é o WS2812 embutido do RP2040-Zero.

**Os pinos de I²C não são arbitrários.** O RP2040 multiplexa o I²C em grupos
fixos de pinos, e `GP10`/`GP11` é um par SDA/SCL válido do I²C1. I²C1
especificamente porque o ChibiOS vem com `RP_I2C_USE_I2C1 TRUE` e
`RP_I2C_USE_I2C0 FALSE` — usar o I²C0 exigiria um `mcuconf.h` customizado só
para ligar o periférico. O `halconf.h` desta pasta liga o `HAL_USE_I2C`.

### Convenção de índices da matriz

O firmware indexa as linhas de cima para baixo, o **inverso** dos nomes do
esquema. Guarde isto ao medir com multímetro:

| Firmware | Esquema | GPIO |
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
| **ENC** | 0 (ROW4) | 5 (COL6) | 5 | botão do encoder |
| SW5  | 1 (ROW3) | 0 (COL1) | 6  | `KC_Q` |
| SW6  | 1 (ROW3) | 1 (COL2) | 7  | `KC_A` |
| SW7  | 1 (ROW3) | 2 (COL3) | 8  | `KC_W` |
| SW8  | 1 (ROW3) | 3 (COL4) | 9  | `KC_D` |
| SW9  | 1 (ROW3) | 4 (COL5) | 10 | `KC_F` |
| SW10 | 2 (ROW2) | 0 (COL1) | 11 | `KC_LSFT` |
| SW11 | 2 (ROW2) | 1 (COL2) | 12 | `KC_Z` |
| SW12 | 2 (ROW2) | 2 (COL3) | 13 | `KC_S` |
| SW13 | 2 (ROW2) | 3 (COL4) | 14 | `KC_X` |
| SW14 | 2 (ROW2) | 4 (COL5) | 15 | `KC_T` |
| SW15 | 2 (ROW2) | 5 (COL6) | 16 | `KC_E` |
| SW16 | 3 (ROW1) | 0 (COL1) | 17 | `KC_G` |
| SW17 | 3 (ROW1) | 1 (COL2) | 18 | `KC_LCTL` |
| SW18 | 3 (ROW1) | 3 (COL4) | 19 | `KC_C` |
| SW19 | 3 (ROW1) | 4 (COL5) | 20 | `KC_SPC` |
| SW20 | 3 (ROW1) | 5 (COL6) | 21 | `KC_B` |

As posições `[0,0]`, `[1,5]` e `[3,2]` não têm switch e são `KC_NO`.

## Perfis

Cinco camadas configuráveis no Vial, com as 20 teclas livres em cada uma:

| # | Perfil | Indicador |
|---|---|---|
| 0 | Jogo | vermelho |
| 1 | 3ds Max | azul |
| 2 | Macros / uso geral | verde |
| 3 | Reservada | roxo |
| 4 | Configuração / manutenção | amarelo |

### Trocar de perfil

Três caminhos, todos chegando ao mesmo lugar:

- **Clique curto no encoder** — próximo perfil.
- **G + B seguradas juntas por 1 s** — o acorde original, mantido porque
  funciona com o encoder desconectado.
- **Menu do OLED → Perfil** — o único caminho que alcança as camadas 3 e 4, que
  os outros dois pulam de propósito.

Os dois ciclos seguem `Jogo → 3ds Max → Macros → Jogo`. Disparar qualquer um
deles estando na camada 3 ou 4 volta para Jogo.

Regras que o acorde garante (em `allien.c`):

- as duas teclas devem descer dentro de `PROFILE_CHORD_SYNC_MS` (50 ms) uma da
  outra;
- o acorde precisa ser mantido por `PROFILE_SWITCH_HOLD_MS` (1000 ms);
- enquanto armado, **nenhum dos keycodes chega ao computador**, então uma troca
  reconhecida nunca vaza um `G` ou `B` perdido no jogo;
- se soltar antes, as duas teclas são entregues normalmente.

O perfil escolhido é gravado na EEPROM, então **sobrevive a reinício**.

> **Ressalva:** as teclas do acorde ficam retidas por até 50 ms e são
> reproduzidas com `register_code16()`. Mantenha-as em keycodes *básicos* —
> layer taps, tap dance e macros do Vial nessas duas posições não reproduzem
> corretamente.

## Encoder e menu do OLED

```
clique curto        -> próximo perfil          (em qualquer tela)
girar               -> navega / ajusta
clique longo 500 ms -> abre o menu, entra no item, confirma, volta
```

O clique curto está reservado para o perfil, então o menu usa o **clique longo**
para confirmar — sem isso não haveria como entrar num item.

Itens do menu: Perfil · Tela · Brilho RGB · Efeito RGB · Encoder · Cronômetro ·
Zerar tempo · Reiniciar.

Telas de descanso, escolhidas em *Tela*: **Status** (perfil, modo do encoder,
RGB, APM, cronômetro), **APM** (janela móvel de 60 s com barra), **Tempo**
(cronômetro) e **Logo**.

O item *Encoder* alterna o giro entre `Menu` (padrão: girar abre e navega o
menu) e `Teclas` (girar manda volume; o menu continua acessível pelo clique
longo).

O painel apaga após 60 s parado (`OLED_TIMEOUT`), porque OLED marca de verdade.
Qualquer tecla ou giro acorda.

> **Duas limitações deliberadas.**
> O `ENCODER_MAP_ENABLE` está **desligado**: com ele ligado, o QMK despacha os
> eventos do encoder direto como keycodes e nunca chama `encoder_update_kb()`,
> que é onde o menu lê o giro. Consequência: **o giro não é remapeável no
> Vial** — o menu é dono dele. Para dar outra função ao fallback, mude os dois
> keycodes em `encoder_update_kb()` no `allien.c`.
> O **botão** também é interceptado por posição na matriz, então o keycode dele
> no keymap nunca é enviado; remapear `[0,5]` no Vial não surte efeito.

## RGB

`RGB_MATRIX` sobre o driver WS2812 vendor (PIO): 20 LEDs, um por tecla.

**A cadeia precisa ser roteada DIN → DOUT seguindo SW1 até SW20** — a mesma
ordem do `LAYOUT`: linha de cima da esquerda para a direita, depois cada linha
abaixo. Esse mapeamento é o `g_led_config` em `allien.c`; se o PCB serpentear,
muda só o primeiro bloco daquela struct.

- A cor do perfil mexe **apenas na matiz**; o efeito e o brilho que você
  escolher no Vial ou no menu ficam preservados.
- Toda escrita do indicador usa variantes `_noeeprom`, então nunca sobrescreve
  suas configurações salvas nem desgasta a flash.
- O `max_brightness` está travado em 120: 20 WS2812 em branco pleno passam de
  1 A, muito além dos 500 mA do USB. Uso real — cor sólida com brilho 80 —
  fica em 120–150 mA.

> **Nível lógico:** o `GP26` entrega 3,3 V, mas um WS2812B em 5 V espera
> 0,7 × VDD = 3,5 V para nível alto. Costuma funcionar, mas se o primeiro LED se
> comportar mal: use SK6812, baixe a alimentação dos LEDs para ~4,3 V com um
> diodo em série, ou acrescente um 74AHCT125.

## Compilar do zero

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

Precisa incluir a **newlib**. O `arm-none-eabi-gcc` do Homebrew **não serve** —
vem sem os headers de libc e o build morre em
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
git checkout encoder-oled-screen
ln -s "$(pwd)/firmware/QMK/VIAL_source/allien" ~/vial-qmk/keyboards/allien
```

Symlink, **nunca cópia**. Copiar cria duas versões dos mesmos arquivos: você
edita no repositório, esquece de copiar, compila a versão velha e não entende
por que a mudança não surtiu efeito.

### 6. Compilar

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk

make allien:vial       # -> allien_vial.uf2    125 KiB · recomendado
make allien:default    # -> allien_default.uf2  89 KiB · QMK puro
```

O `export PATH` precisa vir **antes** do `make`, e em toda sessão de terminal
nova. Um build limpo termina em `Creating UF2 file for deployment` e `[OK]`.
Qualquer `[ERRORS]` é erro real — o QMK trata warnings como erro, então não
existe "compilou com avisos".

> **Uma armadilha que engana:** `brew install qmk/qmk/qmk` pode terminar com
> **código de saída 0 sem instalar nada**, bloqueado pela política de tap trust
> do Homebrew — a saída diz `Would install 1 formula` e para por aí. Os passos
> acima evitam o problema por não dependerem daquele tap.
>
> **Outra:** uma chave de comentário `//` colocada *dentro* de um objeto
> aninhado no `keyboard.json` quebra a validação de schema do QMK, e o build
> acusa `No bootloader specified` — que não aponta para nada relacionado à
> causa. Comentários só funcionam no nível superior.

### Recompilar depois de uma alteração

```sh
export PATH="$HOME/vial-qmk/.toolchain/bin:$HOME/vial-qmk/.venv/bin:$PATH"
cd ~/vial-qmk && make allien:vial
```

Os passos 1 a 5 são uma vez só. Se algo ficar estranho depois de mudar
`keyboard.json` ou `rules.mk`, apague o cache com `rm -rf ~/vial-qmk/.build`.

## Gravar

1. **Duplo toque no RESET.** A placa monta como um pendrive chamado `RPI-RP2`.
2. Copie o `allien_vial.uf2` para dentro dele.
3. Ela reinicia sozinha no firmware novo e o pendrive some — é o esperado.

Se o `RPI-RP2` não aparecer: segure **BOOT**, pressione e solte **RESET**,
depois solte **BOOT**.

> O duplo toque no RESET vem do nosso firmware
> (`RP2040_BOOTLOADER_DOUBLE_TAP_RESET`), não do chip. Numa placa que ainda não
> o tem, use BOOT + RESET na primeira gravação.

## Vial

O `VIAL_KEYBOARD_UID` é próprio e deliberadamente diferente do TOTEM original —
o Vial indexa definições de teclado por UID, e reaproveitar faria o Vial
carregar o layout errado, de 38 teclas.

O combo de destravamento é **`1` + `R`** (`[0,1]` e `[0,4]`).

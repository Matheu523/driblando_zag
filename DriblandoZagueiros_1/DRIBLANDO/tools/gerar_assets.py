#!/usr/bin/env python3
"""
Driblando Zagueiros - gerador dos assets PRÓPRIOS do jogo (sprites e sons).

Todos os arquivos em assets/ foram criados por este script (desenho e síntese
procedurais), portanto são originais do projeto e livres para uso.

Uso:  python tools/gerar_assets.py      (requer Pillow: pip install pillow)
"""
import os, math, random, struct, wave
from PIL import Image, ImageDraw

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SPR = os.path.join(ROOT, 'assets', 'sprites')
SND = os.path.join(ROOT, 'assets', 'sons')

# ---------------------------------------------------------------- personagens (16x24)
def rgb(h):
    h = h.lstrip('#')
    if len(h) == 3: h = ''.join(c * 2 for c in h)
    return tuple(int(h[i:i+2], 16) for i in (0, 2, 4)) + (255,)

def shade(c, f):
    return tuple(max(0, min(255, int(v * f))) for v in c[:3]) + (c[3],)


def draw_char(img, ox, oy, spec, direction, frame):
    skin, hair, style, shirt, pants, extra = spec
    S, H, C, P = rgb(skin), rgb(hair), rgb(shirt), rgb(pants)
    OUT = rgb('#1e1a24'); WHITE = rgb('#ffffff')
    d = ImageDraw.Draw(img)
    def px(x, y, c): d.point((ox + x, oy + y), fill=c)
    def rc(x0, y0, x1, y1, c): d.rectangle((ox + x0, oy + y0, ox + x1, oy + y1), fill=c)
    bob = 1 if frame != 0 else 0
    # sombra
    d.ellipse((ox + 3, oy + 21, ox + 12, oy + 23), fill=(0, 0, 0, 70))
    # pernas
    step = {0: (0, 0), 1: (-1, 1), 2: (1, -1)}[frame]
    robe = style == 'mago'
    if robe:
        rc(4, 14, 11, 21, C); rc(3, 20, 12, 21, shade(C, 0.8))
    else:
        if direction in (0, 3):
            rc(5, 16, 7, 20 + (step[0] < 0), P); rc(8, 16, 10, 20 + (step[1] < 0), P)
            rc(5, 20 + (step[0] < 0), 7, 21 + (step[0] < 0), OUT); rc(8, 20 + (step[1] < 0), 10, 21 + (step[1] < 0), OUT)
        else:
            sx = 1 if frame == 1 else (-1 if frame == 2 else 0)
            rc(6 + sx, 16, 9 + sx, 20, P); rc(6 - sx, 16, 9 - sx, 20, shade(P, 0.85))
            rc(6 + sx, 20, 9 + sx, 21, OUT)
    # tronco
    yb = -bob
    rc(4, 10 + yb, 11, 16 + yb, C)
    rc(4, 16 + yb, 11, 16 + yb, shade(C, 0.75))
    if extra == 'gravata' and direction == 0:
        rc(7, 10 + yb, 8, 15 + yb, rgb('#c0392b'))
    if extra == 'apito' and direction == 0:
        px(7, 12 + yb, rgb('#ddd')); px(8, 13 + yb, rgb('#ddd'))
    if extra == 'mochila' and direction == 3:
        rc(5, 10 + yb, 10, 15 + yb, rgb('#c0392b'))
    if extra == 'mochila' and direction in (1, 2):
        bx = 10 if direction == 1 else 3
        rc(bx, 10 + yb, bx + 2, 15 + yb, rgb('#c0392b'))
    # braços
    if direction in (0, 3):
        ay = 1 if frame == 1 else 0; by = 1 if frame == 2 else 0
        rc(2, 11 + yb + ay, 3, 15 + yb + ay, C); px(2, 16 + yb + ay, S); px(3, 16 + yb + ay, S)
        rc(12, 11 + yb + by, 13, 15 + yb + by, C); px(12, 16 + yb + by, S); px(13, 16 + yb + by, S)
    else:
        ax = 7
        rc(ax, 11 + yb, ax + 1, 15 + yb, shade(C, 0.85)); rc(ax, 16 + yb, ax + 1, 16 + yb, S)
    # cabeça
    hy = 1 + yb
    rc(4, hy + 1, 11, hy + 8, S)
    rc(3, hy + 2, 3, hy + 7, OUT); rc(12, hy + 2, 12, hy + 7, OUT)
    # cabelo
    if style == 'careca':
        rc(3, hy + 2, 4, hy + 5, H); rc(11, hy + 2, 12, hy + 5, H)
    elif style == 'mago':
        d.polygon([(ox + 2, oy + hy + 3), (ox + 13, oy + hy + 3), (ox + 8, oy + hy - 8 + 7)], fill=C)
        rc(2, hy + 2, 13, hy + 3, shade(C, 0.8)); px(8, hy - 1, rgb('#ffd54f'))
        rc(3, hy + 4, 4, hy + 8, H); rc(11, hy + 4, 12, hy + 8, H)
    elif style == 'bone':
        rc(3, hy, 12, hy + 3, C if extra != '' else rgb('#c0392b'))
        if direction == 0: rc(3, hy + 3, 12, hy + 3, shade(C, 0.6))
    else:
        rc(3, hy, 12, hy + 2, H); rc(4, hy - 1, 11, hy - 1, H)
        if direction == 3: rc(3, hy, 12, hy + 7, H)
        if direction == 1: rc(8, hy, 12, hy + 6, H)
        if direction == 2: rc(3, hy, 7, hy + 6, H)
        if style == 'longo':
            rc(3, hy + 2, 4, hy + 10, H); rc(11, hy + 2, 12, hy + 10, H)
        if style == 'rabo' and direction != 0:
            rc(6, hy + 7, 9, hy + 10, H)
        if style == 'coque':
            rc(6, hy - 3, 9, hy - 1, H)
    # rosto
    if direction == 0:
        px(6, hy + 5, OUT); px(9, hy + 5, OUT)
        if extra == 'oculos':
            d.rectangle((ox + 5, oy + hy + 4, ox + 7, oy + hy + 6), outline=OUT); d.rectangle((ox + 8, oy + hy + 4, ox + 10, oy + hy + 6), outline=OUT)
        if extra in ('barba', 'barba_longa'):
            rc(4, hy + 7, 11, hy + 8, H)
        if extra == 'barba_longa':
            rc(5, hy + 8, 10, hy + 12, H); rc(6, hy + 13, 9, hy + 14, H)
    elif direction in (1, 2):
        ex = 5 if direction == 1 else 10
        px(ex, hy + 5, OUT)
        if extra == 'oculos': d.rectangle((ox + ex - 1, oy + hy + 4, ox + ex + 1, oy + hy + 6), outline=OUT)
        if extra in ('barba', 'barba_longa'):
            rc(4 if direction == 1 else 7, hy + 7, 8 if direction == 1 else 11, hy + 8, H)
        if extra == 'barba_longa':
            rc(4 if direction == 1 else 8, hy + 8, 7 if direction == 1 else 11, hy + 13, H)


def sheet(spec, direction, path):
    img = Image.new('RGBA', (48, 24), (0, 0, 0, 0))
    for fr in range(3):
        draw_char(img, fr * 16, 0, spec, direction, fr)
    img.save(path)

def sprites():
    os.makedirs(SPR, exist_ok=True)
    # três craques fictícios (seleção de personagem)
    sheet(('#e0ac69', '#2b1b0e', 'curto', '#2e7d32', '#ffffff', ''), 2, os.path.join(SPR, 'atacante_1.png'))  # Ziguezague
    sheet(('#f1c27d', '#111111', 'careca', '#d32f2f', '#212121', ''), 2, os.path.join(SPR, 'atacante_2.png'))  # Canhão
    sheet(('#8d5524', '#f9c80e', 'coque', '#ff8f00', '#1a237e', ''), 2, os.path.join(SPR, 'atacante_3.png'))   # Foguete
    sheet(('#c68642', '#111111', 'curto', '#1565c0', '#0d47a1', ''), 1, os.path.join(SPR, 'zagueiro.png'))
    sheet(('#e0ac69', '#5d4037', 'curto', '#43a047', '#212121', ''), 1, os.path.join(SPR, 'goleiro.png'))

    # bola 8x8
    b = Image.new('RGBA', (8, 8), (0, 0, 0, 0)); d = ImageDraw.Draw(b)
    d.ellipse((0, 0, 7, 7), fill=rgb('#ffffff'), outline=rgb('#333333'))
    d.rectangle((3, 3, 4, 4), fill=rgb('#222222')); d.point((1, 2), fill=rgb('#222222')); d.point((6, 5), fill=rgb('#222222'))
    b.save(os.path.join(SPR, 'bola.png'))

    # moeda 4 quadros 8x8 (girando)
    m = Image.new('RGBA', (32, 8), (0, 0, 0, 0)); d = ImageDraw.Draw(m)
    for i, w in enumerate((3, 2, 1, 2)):
        cx = i * 8 + 4
        d.ellipse((cx - w, 0, cx + w - 1 if w > 1 else cx, 7), fill=rgb('#f9c80e'), outline=rgb('#b8860b'))
        if w >= 2: d.line((cx - 1 + (w == 3), 2, cx - 1 + (w == 3), 5), fill=rgb('#fff3b0'))
    m.save(os.path.join(SPR, 'moeda.png'))

    # trave/rede vista de cima: 24x96
    g = Image.new('RGBA', (24, 96), (0, 0, 0, 0)); d = ImageDraw.Draw(g)
    d.rectangle((4, 4, 23, 91), fill=(230, 230, 230, 120))
    for y in range(6, 92, 4): d.line((5, y, 23, y), fill=(255, 255, 255, 170))
    for x in range(8, 24, 4): d.line((x, 5, x, 91), fill=(255, 255, 255, 170))
    d.rectangle((0, 0, 23, 4), fill=rgb('#f4f4f4')); d.rectangle((0, 91, 23, 95), fill=rgb('#f4f4f4'))
    d.rectangle((0, 0, 4, 95), fill=rgb('#f4f4f4'))
    g.save(os.path.join(SPR, 'gol.png'))

    # arquibancada com torcida: 64x30 (repetida na horizontal)
    r = random.Random(7)
    a = Image.new('RGBA', (64, 30), rgb('#5b6470')); d = ImageDraw.Draw(a)
    for y in (0, 10, 20):
        d.rectangle((0, y + 8, 63, y + 9), fill=rgb('#3d434c'))
        for x in range(1, 64, 4):
            c = r.choice(['#d32f2f', '#ffffff', '#1565c0', '#f9c80e', '#d32f2f', '#2e7d32'])
            skin = r.choice(['#f1c27d', '#c68642', '#8d5524', '#ffdbac'])
            d.rectangle((x, y + 4, x + 2, y + 7), fill=rgb(c))
            d.rectangle((x, y + 1, x + 2, y + 3), fill=rgb(skin))
    a.save(os.path.join(SPR, 'arquibancada.png'))

# ---------------------------------------------------------------- sons (WAV 22 kHz, 16 bits)
RATE = 22050

def save_wav(name, samples):
    os.makedirs(SND, exist_ok=True)
    with wave.open(os.path.join(SND, name), 'w') as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(RATE)
        w.writeframes(b''.join(struct.pack('<h', int(max(-1, min(1, s)) * 30000)) for s in samples))

def tone(freq, dur, vol=0.5, wave_type='square', decay=True):
    out = []
    n = int(RATE * dur)
    for i in range(n):
        t = i / RATE
        f = freq(t) if callable(freq) else freq
        ph = (t * f) % 1.0
        v = (1 if ph < 0.5 else -1) if wave_type == 'square' else math.sin(2 * math.pi * ph)
        env = (1 - i / n) if decay else 1
        out.append(v * vol * env)
    return out

def noise(dur, vol=0.5, seed=1, lowpass=0.0):
    r = random.Random(seed); out = []; prev = 0
    n = int(RATE * dur)
    for i in range(n):
        v = r.uniform(-1, 1)
        prev = prev * lowpass + v * (1 - lowpass)
        out.append(prev * vol * (1 - i / n))
    return out

def sons():
    save_wav('chute.wav', [a + b for a, b in zip(noise(0.12, 0.8, 2, 0.6), tone(lambda t: 160 - 600 * t, 0.12, 0.6, 'sine'))])
    save_wav('moeda.wav', tone(988, 0.06, 0.3) + tone(1319, 0.16, 0.3))
    save_wav('drible.wav', tone(lambda t: 500 + 3000 * t, 0.12, 0.25))
    save_wav('falta.wav', tone(lambda t: 220 - 300 * t, 0.35, 0.4))
    apito = tone(lambda t: 2600 + 180 * math.sin(2 * math.pi * 30 * t), 0.5, 0.25, 'sine', decay=False)
    save_wav('apito.wav', [s * min(1, (len(apito) - i) / 800) for i, s in enumerate(apito)])
    torcida = noise(1.6, 0.6, 5, 0.85)
    festa = tone(lambda t: 523 if t < 0.15 else (659 if t < 0.3 else 784), 0.6, 0.25)
    save_wav('gol.wav', [a + (festa[i] if i < len(festa) else 0) for i, a in enumerate(torcida)])
    save_wav('menu.wav', tone(880, 0.05, 0.2))
    # ambiente de torcida (loop de 4 s, sem fade)
    r = random.Random(9); prev = 0; amb = []
    for i in range(RATE * 4):
        prev = prev * 0.92 + r.uniform(-1, 1) * 0.08
        swell = 0.55 + 0.45 * math.sin(2 * math.pi * i / (RATE * 4))
        amb.append(prev * 1.6 * swell)
    save_wav('torcida.wav', amb)

if __name__ == '__main__':
    sprites(); sons()
    print('assets gerados em', os.path.join(ROOT, 'assets'))

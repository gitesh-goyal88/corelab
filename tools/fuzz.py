import json, random, subprocess, sys, time, urllib.request, urllib.parse, os
from concurrent.futures import ThreadPoolExecutor
BIN = sys.argv[1]; PORT = int(sys.argv[2]); ROOT = sys.argv[3]; SEED = int(sys.argv[4]) if len(sys.argv) > 4 else 1
random.seed(SEED)
log = open('/tmp/fuzz_server.log', 'w')
srv = subprocess.Popen([BIN, str(PORT)], cwd=ROOT, stdout=log, stderr=log)
base = f'http://127.0.0.1:{PORT}'
for _ in range(100):
    try: urllib.request.urlopen(base + '/api/curriculum', timeout=2).read(); break
    except Exception: time.sleep(0.3)
def get(path):
    try:
        with urllib.request.urlopen(base + path, timeout=60) as r: return r.status, r.read()
    except urllib.error.HTTPError as e: return e.code, e.read()
    except Exception as e: return -1, str(e).encode()
cur = json.loads(get('/api/curriculum')[1])
topics = [t['name'] for s in cur for sub in s['subjects'] for t in sub['topics']]
print('topics', len(topics), 'unique', len(set(topics)))
bad = []; counts = {'sim_avail': 0, 'phys_avail': 0, 'neither': []}
SIM_IN = ['', ' ', '0', '-1', '1e308', '-1e308', 'nan', 'inf', '-inf', '1,2,3', 'a,b,c', ',,,', '1,,2', '0,0,0,0,0,0', '1e-300', '5,3,8,1,9,2,7', '99999999999', '1' * 400,
          ','.join(str(random.randint(-50, 50)) for _ in range(500)), 'x' * 5000, '％ü€𝔘', '1;2;3', '-0,-0', '1e5,1e5,1e5,1e5', '0.0000001,0.0000001', '3,1,2\n4,5,6', '1 2 3 4 5', '2,-2,2,-2,2,-2']
for _ in range(15): SIM_IN.append(','.join(random.choice(['0', '1', '-1', '2.5', '1e6', '-1e6', '7', '100', '0.001', '1e-9']) for _ in range(random.randint(1, 9))))
OPTS = ['0', '1', '2', '3', '4', '5', '-1', '99', 'abc', '', '999999999999']
def chk(path):
    st, body = get(path)
    if st != 200: return (path[:150], 'status %s' % st)
    try: j = json.loads(body)
    except Exception as e: return (path[:150], 'bad json %s' % e)
    return j
def do_topic(t):
    res = []; sim = False; ph = False
    q = urllib.parse.quote(t)
    rr = chk(f'/api/reallife?topic={q}')
    if isinstance(rr, tuple): res.append(rr)
    elif not rr.get('available') or not rr.get('uses'): res.append((t, 'no real-life entry'))
    for opt in ['2', '3', '4']:
        for inp in ['', '5', '-1', '1e308', 'a|b', '0,0,0']:
            r2 = chk(f'/api/simulate?topic={q}&opt={opt}&input={urllib.parse.quote(inp)}')
            if isinstance(r2, tuple): res.append(r2)
    for inp in SIM_IN:
        for opt in (['0', '1'] if random.random() < .5 else random.sample(OPTS, 3)):
            r = chk(f'/api/simulate?topic={q}&opt={opt}&input={urllib.parse.quote(inp)}')
            if isinstance(r, tuple): res.append(r)
            elif r.get('available'): sim = True
    r = chk(f'/api/simulate?topic={q}&opt=0&input=')
    r0 = chk(f'/api/physics?topic={q}')
    params = []
    if not isinstance(r0, tuple) and r0.get('available'):
        ph = True; params = r0['params']
    elif isinstance(r0, tuple): res.append(r0)
    if ph:
        def vec(f): return ','.join(('%.6f' % f(p)) if f(p) is not None else 'nan' for p in params)
        vs = [vec(lambda p: p['min']), vec(lambda p: p['max']), vec(lambda p: p['def']), vec(lambda p: p['min'] - 1e3), vec(lambda p: p['max'] + 1e9), vec(lambda p: 0), vec(lambda p: -p['max']), vec(lambda p: 1e-12), vec(lambda p: 1e300), 'nan,nan,nan', 'inf,-inf', '', ',,,', 'abc', '1', '1,2', '1,2,3,4,5,6,7,8,9,10,11,12', '-0,-0,-0']
        for _ in range(40):
            vs.append(','.join('%.6f' % random.uniform(p['min'] - (p['max'] - p['min']) * .1, p['max'] + (p['max'] - p['min']) * .1) for p in params))
        for i in range(len(params)):  # sweep each slider alone across its range
            for k in range(11):
                vs.append(','.join('%.6f' % (p['min'] + (p['max'] - p['min']) * (k / 10) if j == i else p['def']) for j, p in enumerate(params)))
        for v in vs:
            r = chk(f'/api/physics?topic={q}&p={urllib.parse.quote(v)}')
            if isinstance(r, tuple): res.append(r)
    for t2 in [t + 'x', '', '../etc/passwd', 'A' * 3000]:
        chk(f'/api/simulate?topic={urllib.parse.quote(t2)}&opt=0&input=1'); chk(f'/api/physics?topic={urllib.parse.quote(t2)}&p=1')
    return t, sim, ph, res
with ThreadPoolExecutor(6) as ex:
    for t, sim, ph, res in ex.map(do_topic, topics):
        counts['sim_avail'] += sim; counts['phys_avail'] += ph
        if not sim and not ph: counts['neither'].append(t)
        bad += [(t,) + r for r in res]
alive = srv.poll() is None
srv.terminate(); time.sleep(.5); log.close()
slog = open('/tmp/fuzz_server.log', errors='replace').read()
san = ('AddressSanitizer' in slog) or ('runtime error' in slog)
print('simulate topics', counts['sim_avail'], 'physics topics', counts['phys_avail'], 'with neither:', counts['neither'])
print('bad responses', len(bad)); [print(' ', b) for b in bad[:15]]
print('server alive at end:', alive, '| sanitizer messages in log:', san)
if san: print(slog[:3000])
sys.exit(0 if (alive and not san and not bad) else 1)

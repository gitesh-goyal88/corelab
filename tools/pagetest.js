const { JSDOM, VirtualConsole } = require('/home/claude/tools/node_modules/jsdom');
const PORT = process.argv[2] || 8080, base = `http://127.0.0.1:${PORT}/`;
const errors = [];
(async () => {
  const vc = new VirtualConsole();
  vc.on('jsdomError', e => errors.push('jsdomError: ' + (e.detail || e.message || e)));
  vc.on('error', e => errors.push('console.error: ' + e));
  const dom = await JSDOM.fromURL(base, { runScripts: 'dangerously', resources: 'usable', pretendToBeVisual: true, virtualConsole: vc,
    beforeParse(w) { w.fetch = (u, o) => fetch(new URL(u, base), o); w.scrollTo = () => {}; w.Element.prototype.scrollIntoView = function () {}; } });
  const w = dom.window, d = w.document;
  w.addEventListener('error', e => errors.push('window.error: ' + e.message));
  w.addEventListener('unhandledrejection', e => errors.push('unhandledrejection: ' + e.reason));
  const banners = []; const orig = w.showErr; w.showErr = m => { banners.push(m); };
  const sleep = ms => new Promise(r => setTimeout(r, ms));
  const waitFor = async (f, ms = 15000) => { const t0 = Date.now(); while (Date.now() - t0 < ms) { try { if (f()) return true; } catch (e) {} await sleep(20); } return false; };
  await waitFor(() => w.eval('typeof DATA!=="undefined" && DATA && DATA.length'));
  const cur = JSON.parse(w.eval('JSON.stringify(DATA)'));
  const scr = () => d.getElementById('screen');
  let nDemo = 0, nTopics = 0, nStep = 0, nPhys = 0, nEmpty = 0, nSliderMoves = 0, nNext = 0; const problems = [];
  const open = async (sem, sub, topic, opt) => {
    w.eval(`state.sem=${sem.id};state.subject=${JSON.stringify(sub.name)};state.topic=${JSON.stringify(topic.name)};state.opt=${opt};go('simulate')`);
    await waitFor(() => !/Loading…/.test(scr().innerHTML) || false, 20000);
    await sleep(10);
  };
  const stepThrough = async (name) => {
    const btn = id => d.getElementById(id);
    await waitFor(() => btn('run'));
    btn('run').click();
    if (!(await waitFor(() => btn('next') && d.getElementById('note')))) { problems.push(name + ': no step panel after Run'); return; }
    const ctr = () => (btn('ctr') || {}).textContent;
    let guard = 0; const first = ctr();
    while (guard++ < 80) { btn('next').click(); nNext++; await sleep(0); }
    const lastc = ctr(); if (!d.getElementById('note').textContent.trim()) problems.push(name + ': empty note at last step');
    guard = 0; while (guard++ < 80) { btn('prev').click(); }
    if (ctr() !== first) problems.push(`${name}: counter after prev x80 '${ctr()}' != '${first}'`);
    const svg = d.getElementById('cirsvg'); if (svg && !svg.querySelector('svg')) problems.push(name + ': cirsvg empty');
    const def = btn('def'); if (def) { def.click(); await waitFor(() => btn('next')); }
    const chips = d.querySelectorAll('#pre .chip'); for (const c of chips) { c.click(); await waitFor(() => btn('next')); btn('next').click(); }
    await sleep(60); nStep++;
  };
  const sliders = async (name) => {
    const ins = [...d.querySelectorAll('#sliders input[type=range]')];
    if (!ins.length) { problems.push(name + ': physics but no sliders'); return; }
    const fire = async (el, v) => { el.value = v; el.dispatchEvent(new w.Event('input', { bubbles: true })); nSliderMoves++; await sleep(15); };
    for (const el of ins) for (const v of [el.min, el.max, (+el.min + +el.max) / 2, el.defaultValue]) await fire(el, v);
    await sleep(60); await waitFor(() => d.getElementById('vis') && d.getElementById('vis').innerHTML.length > 50);
    const vaz = d.getElementById('vaz'); if (vaz) { for (const v of [-180, 0, 180]) { vaz.value = v; vaz.dispatchEvent(new w.Event('input', { bubbles: true })); } const vel = d.getElementById('vel'); for (const v of [vel.min, vel.max]) { vel.value = v; vel.dispatchEvent(new w.Event('input', { bubbles: true })); } }
    const rst = d.getElementById('reset'); if (rst) rst.click();
    await sleep(80);
    if (!d.querySelector('#vis svg') && !d.querySelector('#vis')) problems.push(name + ': no plot');
    if (!d.querySelector('#refs li')) problems.push(name + ': no reference links');
    nPhys++;
  };
  const only = process.env.ONLY ? process.env.ONLY.split('|') : null;
  for (const sem of cur) for (const sub of sem.subjects) for (const topic of sub.topics) {
    if (only && !only.includes(topic.name)) continue;
    nTopics++; const name = `${sub.name} / ${topic.name}`; const b0 = banners.length, e0 = errors.length;
    await open(sem, sub, topic, 0);
    if (!(await waitFor(() => d.querySelector('#rl .panel'), 6000))) problems.push(name + ': no real-life panel');
    if (d.getElementById('sliders')) await sliders(name);
    else if (d.getElementById('run')) {
      const progs = d.querySelectorAll('input[name=prog]').length;
      await stepThrough(name);
      for (let k = 1; k < progs; k++) { await open(sem, sub, topic, k); await stepThrough(name + ' [prog ' + k + ']'); }
      await open(sem, sub, topic, 0); await waitFor(() => d.getElementById('rlgo') || d.querySelector('#rl .panel'), 6000); await sleep(150);
      const go = d.getElementById('rlgo'); if (go) { go.click(); await waitFor(() => d.getElementById('run') && d.getElementById('rlback') === null ? true : d.getElementById('run')); await stepThrough(name + ' [real-life demo]'); if (!d.querySelector('#rlback')) problems.push(name + ': real-life demo button did not switch'); nDemo++; }
    } else { nEmpty++; problems.push(name + ': neither sim nor sliders: ' + scr().textContent.slice(0, 80)); }
    if (banners.length > b0) problems.push(name + ': banner ' + banners.slice(b0).join('|'));
    if (errors.length > e0) problems.push(name + ': errors ' + errors.slice(e0).join('|'));
  }
  console.log({ topics: nTopics, stepTopics: nStep, physicsTopics: nPhys, empty: nEmpty, sliderMoves: nSliderMoves, nextClicks: nNext, realLifeDemos: nDemo });
  console.log('problems:', problems.length); problems.slice(0, 30).forEach(p => console.log(' -', p));
  w.close(); process.exit(problems.length ? 1 : 0);
})();

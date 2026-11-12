const { chromium } = require('/opt/npm-tools/node_modules/playwright');
(async () => {
  const only = process.argv[2];
  const b = await chromium.launch({ executablePath: '/opt/pw-browsers/chromium-1243/chrome-linux64/chrome' });
  const p = await b.newPage({ viewport: { width: 700, height: 600 }, deviceScaleFactor: 1.5 });
  await p.goto('http://localhost:8080/');
  await p.waitForTimeout(500);
  const cur = await p.evaluate(() => fetch('/api/curriculum').then(r => r.json()));
  const ws = cur.flatMap(s => s.subjects).find(s => s.name === 'WorkShop');
  const topics = ws.topics.map(t => t.name).filter(n => !only || n.includes(only));
  const jobs = []; for (const n of topics) { jobs.push([n,0]); if (n.startsWith('Dovetail')) jobs.push([n,1]); }
  for (const [name, opt] of jobs) {
    const m = await p.evaluate(([n,o]) => fetch('/api/simulate?topic=' + encodeURIComponent(n) + '&opt=' + o + '&input=').then(r => r.json()), [name, opt]);
    const safe = name.replace(/[^A-Za-z0-9]+/g, '_') + (opt ? '_opt' + opt : '');
    const n = m.steps.length;
    for (let i = 0; i < n; i++) {
      await p.evaluate(([m, i]) => {
        const st = m.steps[i];
        document.body.innerHTML = '<div id="cirsvg" style="width:640px;background:#fff;padding:4px">' +
          cirSVG(m.diagram, (st.hl && st.hl.length) ? st.hl : (m.diagram.map[st.line] || [])) + '</div>';
      }, [m, i]);
      await (await p.$('#cirsvg')).screenshot({ path: `${safe}_${String(i).padStart(2, '0')}.png` });
    }
    console.log(name, n, m.diagram ? m.diagram.w + 'x' + m.diagram.h : 'NO DIAGRAM');
  }
  await b.close();
})();

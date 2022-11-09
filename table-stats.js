const fs = require("fs");
const table = fs.readFileSync(process.argv[2]);

const dist = new Array(255).fill(0);
let max = 0;
let sum = 0;
table.forEach(x => {
    dist[x]++
    max = Math.max(max, x);
    sum += x;
});

console.log(dist.slice(0, max + 1).map((x, i) => `${x}: ${i} (${Number(x * 100 / table.length).toFixed(1)}%)`).join('\n'));
console.log("expected value = " + (sum / table.length));
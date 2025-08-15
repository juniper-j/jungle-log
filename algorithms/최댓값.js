const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n');

let nums = input.map(Number);
let max_value = Math.max(...nums);
let max_index = nums.indexOf(max_value) + 1;

console.log(max_value);
console.log(max_index);

const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n');

const [N, X] = input[0].split(' ').map(Number);
const nums = input[1].split(' ').map(Number);

let result = [];
for (let i = 0; i < N; i++) {
    if (nums[i] < X) {
        result.push(nums[i]);
    }
}

console.log(result.join(' '));

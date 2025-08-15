const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n');

const T = input[0];
for (let i = 1; i <= T; i++) {
    let arr = input[i].split(' ');
    console.log(parseInt(arr[0]) + parseInt(arr[1]));
}
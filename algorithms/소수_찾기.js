const readline = require('readline');

(async () => {
	let rl = readline.createInterface({ input: process.stdin });

    const input = [];   // 입력을 저장할 배열
	
	for await (const line of rl) {
		input.push(line.trim());    // 입력된 라인을 저장
        if (input.length === 2) {
            rl.close();
            break;  // 두 줄 입력 후 종료 (문제 조건: N과 수열)
        }
	}
	
    // 첫 줄은 수의 개수 N, 두 번째 줄은 공백으로 구분된 숫자 목록
    const numbers = input[1].split(' ').map(Number);

    // 소수 판별 함수
    const isPrime = (n) => {
        if (n < 2) return false; // 0과 1은 소수가 아님
        for (let i = 2; i * i <= n; i++) {
            if (n % i === 0) return false; // 나누어 떨어지면 소수가 아님
        }
        return true; // 소수인 경우
    };

    // 소수의 개수를 세기
    let count = 0;
    for (const num of numbers) {
        if (isPrime(num)) {
            count++; // 소수이면 카운트 증가
        }
    }

    console.log(count);
	process.exit();

})();
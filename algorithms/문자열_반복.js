const readline = require('readline');

(async () => {
    let rl = readline.createInterface({ input: process.stdin });

    const input = [];       // 입력을 저장할 배열
    
    for await (const line of rl) {
        input.push(line.trim());    // 입력된 라인을 저장
        if (input.length === Number(input[0] + 1)) {
            rl.close();     // 기대한 입력 수 만큼 받은 뒤 종료
            break;  
        }
    }
    
    const T = Number(input[0]);     // 첫 줄은 테스트 케이스의 수
    const cases = input.slice(1);   // 각 테스트 케이스 줄들

    // 문자열 반복 함수
    const repeatString = (line) => {
        const [rStr, S] = line.split(' ');
        const R = Number(rStr);
        let result = '';

        for (let ch of S) {
            result += ch.repeat(R); // 각 문자 S[i]를 R번 반복
        }
        return result;
    };

    // 각 테스트 케이스에 대해 문자열 반복 수행
    for (const line of cases) {
        console.log(repeatString(line));
    }

    process.exit();
})();
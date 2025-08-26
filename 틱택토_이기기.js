/* 
 * 틱택토 이기기 (BOJ 9290)
 * https://www.acmicpc.net/problem/9290
 * 
 * [Problem & Solvings]
 * - 3x3 틱택토 판에서 남규가 다음 턴에 'x' 또는 'o' 중 하나를 둔다.
 * - 그 수 하나로 무조건 승리 가능한 위치가 존재한다고 보장됨.
 * - 그 승리 수를 두었을 때의 판 상태를 출력해야 함.
 * 
 * [Pseudocode]
 * 1. 테스트 케이스 수 T 입력
 * 2. 각 테스트 케이스마다:
 *    - 3줄의 판 상태 입력
 *    - 다음 수로 둘 문자 입력 (ch)
 * 3. 모든 빈칸('.')에 ch를 시뮬레이션으로 넣고:
 *    - 그 수를 둔 뒤 승리하는지 판단
 *    - 승리한다면 해당 위치에 ch를 두고 출력
 * 4. "Case x:" 와 함께 새로운 게임판 상태를 출력
 * 
 * [Time Complexity]
 * - 각 테스트 케이스당:
 *   - 최대 9칸 중 빈칸 확인 O(9)
 *   - 각 시뮬레이션마다 승리 체크 O(8) (3행, 3열, 2대각선)
 * - 총 시간 복잡도: O(T * 9 * 8) = O(T)
 */

// Run by Node.js
const readline = require("readline");

(async () => {
    const rl = readline.createInterface({ input: process.stdin });
    const input = [];

    for await (const line of rl) {
        input.push(line.trim());
        const T = Number(input[0]);
        if (input.length === 4 * T + 1) {
            rl.close();
            break;
        }
    }

    const T = Number(input[0]);
    const testcases = input.slice(1);

    const isWin = (board, ch) => {
        for (let i = 0; i < 3; i++) {
            // 가로, 세로 체크
            if ((board[i][0] === ch && board[i][1] === ch && board[i][2] === ch) ||
                (board[0][i] === ch && board[1][i] === ch && board[2][i] === ch)) {
                return true;
            }
        }

        if ((board[0][0] === ch && board[1][1] === ch && board[2][2] === ch) ||
            (board[0][2] === ch && board[1][1] === ch && board[2][0] === ch)) {
            return true;
        }
        return false;
    };

    for (let t = 0; t < T; t++) {
        const board = testcases.slice(t * 4, t * 4 + 3).map(line => line.split(''));
        const ch = testcases[t * 4 + 3];

        let found = false;

        outer: for (let i = 0; i < 3; i++) {
            for (let j = 0; j < 3; j++) {
                if (board[i][j] === '-') {
                    // 빈칸에 ch를 넣어 시뮬레이션
                    board[i][j] = ch;
                    if (isWin(board, ch)) {
                        found = true;
                        break outer; // 승리하면 반복 종료
                    }
                    // 원래대로 돌려놓기
                    board[i][j] = '-';
                }
            }
        }

        console.log(`Case ${t + 1}:`);
        console.log(board.map(row => row.join('')).join('\n'));
    }

    process.exit();
}
)();
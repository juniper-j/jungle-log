/* 
 * 삼각형 (BOJ 1925)
 * https://www.acmicpc.net/problem/1925
 * 
 * [Problem & Solvings]
 * - 세 점이 주어졌을 때, 삼각형을 만들 수 있는지 판단하고, 
 * - 만들 수 있다면 변 길이(이등변 여부)와 각도(예각/직각/둔각)에 따라 종류를 출력한다.
 * - 제곱 거리 비교를 통해 가장 긴 변과 나머지를 비교 → 피타고라스 정리 활용
 *
 * [Pseudocode]
 * 1. 세 점 입력받기
 * 2. 변 세 개가 일직선이면 (= 면적이 0 이면) → X 출력
 * 3. 세 변의 제곱 길이 계산 → AB, BC, AC
 * 4. 정렬하여 가장 긴 변이 마지막이 되도록
 * 5. 세 변의 길이가 모두 같으면 → JungTriangle
 * 6. 두 변이 같은 경우 → 이등변 여부 판별
 * 7. 가장 긴 변² > 나머지 합 → 둔각 / = → 직각 / < → 예각
 *
 * [Time Complexity]
 * - 입력은 고정 세 줄이므로 O(1)
 * - 정렬도 고정 3개에 대해 이루어지므로 O(1)
 */

// Run by Node.js
const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

const input = [];

rl.on("line", (line) => {
    input.push(line.trim());
    if (input.length === 3) {
        rl.close();
    }
});

rl.on("close", () => {
    const [A, B, C] = input.map(line => line.split(" ").map(Number));

    // 일직선 여부 판단
    const area = (A[0] * (B[1] - C[1])
                + B[0] * (C[1] - A[1])
                + C[0] * (A[1] - B[1]));

    if (area === 0) {
        console.log('X');
        return;
    }

    // 세 변의 제곱 길이 계산 함수
    const distSquared = (p1, p2) => {
        const dx = p1[0] - p2[0];
        const dy = p1[1] - p2[1];
        return dx * dx + dy * dy;
    }

    // 변의 제곱 길이 계산
    let a = distSquared(B, C);  // BC Squared
    let b = distSquared(A, C);  // AC Squared
    let c = distSquared(A, B);  // AB Squared

    // 변 길이 오름차순으로 정렬
    const sides = [a, b, c].sort((x, y) => x - y);  // [short, mid, long]
    const [x, y, z] = sides;

    // 정삼각형, 이등변 삼각형 여부 판단
    const isEquilateral = x === y && y === z;
    const isIsosceles = x === y || y === z || x === z;

    if (isEquilateral) {
        console.log("JungTriangle");
    } else if (isIsosceles) {
        if (z > x + y) {
            console.log("Dunkak2Triangle");
        } else if (z === x + y) {
            console.log("Jikkak2Triangle");
        } else {
            console.log("Yeahkak2Triangle");
        }
    } else {
        if (z > x + y) {
            console.log("DunkakTriangle");
        } else if (z === x + y) {
            console.log("JikkakTriangle");
        } else {
            console.log("YeahkakTriangle");
        }
    }
});
/*
DNA (BOJ 1969)
https://www.acmicpc.net/problem/1969

[Problem & Solvings]
- DNA의 각 위치의 뉴클레오티드 문자가 다른 수 만큼을 Hamming Distance 라고 함
- N개의 DNA가 주어졌을 때, Hamming Distance의 합이 가장 작은 DNA와 그 길이를 출력 (단, 목록 중에서 찾는 것이 아님)
- Greedy 핵심: 각 위치별로 가장 많이 등장하는 문자를 선택하여 합을 최소화

[Pseudocode]
1. 입력 받기
2. 열 단위로 각 자리의 문자 빈도 계산
3. 결과 DNA 만들기
4. Hamming Distance 계산
5. 출력

[Time Complexity]
- 입력 처리: O(N)
- 각 위치별 빈도 계산: O(N * M) (N: DNA 개수, M: 길이)
- 총 시간 복잡도: O(N * M)
- 총 공간 복잡도: O(N * M)
*/

// Run by Node.js
const readline = require('readline');

(async () => {
    const rl = readline.createInterface({ input: process.stdin });
    const input = [];

    // 입력 받기
    for await (const line of rl) {
        input.push(line.trim());    // 입력된 라인을 저장
        const [N] = input[0].split(' ').map(Number);
        if (input.length === N + 1) { 
            rl.close(); // N개의 DNA 입력을 다 받았으면 종료
            break;  
        }
    }
  
    // ---------- 입력 ----------
    const [N, M] = input[0].split(' ').map(Number); // N: 문자열 개수, M: 문자열 길이
    const dnaList = input.slice(1); // N개의 DNA 문자열
  
    // ---------- 상태 ----------
    const nucleotides = ['A', 'C', 'G', 'T'];   // 우선순위 판단용 배열
    let resultDNA = ''; // 결과 DNA 문자열
    let totalHD = 0;    // 총 Hamming Distance

    // ---------- 각 열(column)마다 반복 ----------
    for (let i = 0; i < M; i++) {
        // 각 뉴클레오타이드의 빈도수 초기화
        const counts = { A: 0, C: 0, G: 0, T: 0 }; 

        // 각 DNA 문자열의 i번째 문자 빈도 계산
        for (const dna of dnaList) {
            const char = dna[i];
            counts[char]++;
        }

        // 가장 많이 등장하는 문자 찾기 (동률 시 A > C > G > T 우선)
        let maxChar = 'A'; // 기본값
        let maxCount = counts['A'];

        for (const nt of nucleotides.slice(1)) {
            if (counts[nt] > maxCount) {
                maxChar = nt;
                maxCount = counts[nt];
            }
        }

        resultDNA += maxChar; // 결과 DNA에 추가
        totalHD += (N - maxCount); // Hamming Distance 계산
    }

    // ---------- 출력 ----------
    console.log(resultDNA);
    console.log(totalHD);

    process.exit(0); // Node.js 종료
  })();

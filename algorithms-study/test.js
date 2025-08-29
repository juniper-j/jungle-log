function minCostColoring(costs) {
    const n = costs.length;
    if (n === 0) return 0;
  
    const dp = Array.from({ length: n }, (_, i) => [...costs[i]]);
  
    for (let i = 1; i < n; i++) {
      dp[i][0] += Math.min(dp[i - 1][1], dp[i - 1][2]);
      dp[i][1] += Math.min(dp[i - 1][0], dp[i - 1][2]);
      dp[i][2] += Math.min(dp[i - 1][0], dp[i - 1][1]);
    }
  
    return Math.min(...dp[n - 1]);
  }

console.log(minCostColoring([[17, 2, 17], [16, 16, 5], [14, 3, 19]])); // 10
console.log(minCostColoring([[7, 6, 2]])); // 2
console.log(minCostColoring([])); // 0

  
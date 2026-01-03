import cv2
import numpy as np
import collections

class Dinic:
    def __init__(self, n):
        self.n = n
        self.graph = [[] for _ in range(n)]
        self.level = []

    def add_edge(self, u, v, capacity):
        # Forward edge and residual (backward) edge
        self.graph[u].append([v, capacity, len(self.graph[v])])
        self.graph[v].append([u, 0, len(self.graph[u]) - 1])

    def bfs(self, s, t):
        self.level = [-1] * self.n
        self.level[s] = 0
        queue = collections.deque([s])
        while queue:
            u = queue.popleft()
            for v, cap, rev_idx in self.graph[u]:
                if cap > 0 and self.level[v] < 0:
                    self.level[v] = self.level[u] + 1
                    queue.append(v)
        return self.level[t] >= 0

    def dfs(self, u, t, flow, ptr):
        if u == t or flow == 0:
            return flow
        for i in range(ptr[u], len(self.graph[u])):
            ptr[u] = i
            v, cap, rev_idx = self.graph[u][i]
            if self.level[v] == self.level[u] + 1 and cap > 0:
                pushed = self.dfs(v, t, min(flow, cap), ptr)
                if pushed > 0:
                    self.graph[u][i][1] -= pushed
                    self.graph[v][rev_idx][1] += pushed
                    return pushed
        return 0

    def max_flow(self, s, t):
        max_f = 0
        while self.bfs(s, t):
            ptr = [0] * self.n
            while True:
                pushed = self.dfs(s, t, float('inf'), ptr)
                if pushed == 0:
                    break
                max_f += pushed
        return max_f

    def get_reachable(self, s):
        """Finds all nodes reachable from S in the residual graph."""
        reachable = [False] * self.n
        queue = collections.deque([s])
        reachable[s] = True
        while queue:
            u = queue.popleft()
            for v, cap, rev_idx in self.graph[u]:
                if cap > 0 and not reachable[v]:
                    reachable[v] = True
                    queue.append(v)
        return reachable

def segment_image_scratch(img_path, scale=0.2, lam=10):
    # 1. Load and downscale for performance
    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    img = cv2.resize(img, (0,0), fx=scale, fy=scale)
    h, w = img.shape
    img_norm = img / 255.0

    # 2. Build Graph
    # Nodes: S (0), T (1), Pixels (2 to h*w + 1)
    S, T = 0, 1
    num_pixels = h * w
    dinic = Dinic(num_pixels + 2)

    def get_id(r, c): return r * w + c + 2

    for r in range(h):
        for c in range(w):
            u = get_id(r, c)
            val = img_norm[r, c]
            
            # T-links: Bias towards white as foreground
            dinic.add_edge(S, u, val) 
            dinic.add_edge(u, T, 1.0 - val)

            # N-links: Horizontal and Vertical neighbors
            for dr, dc in [(0, 1), (1, 0)]:
                nr, nc = r + dr, c + dc
                if nr < h and nc < w:
                    v = get_id(nr, nc)
                    # Penalty for intensity difference
                    diff = abs(img_norm[r, c] - img_norm[nr, nc])
                    weight = lam * np.exp(-diff) 
                    dinic.add_edge(u, v, weight)
                    dinic.add_edge(v, u, weight)

    # 3. Solve
    dinic.max_flow(S, T)
    reachable = dinic.get_reachable(S)

    # 4. Reconstruct Mask
    mask = np.zeros((h, w), dtype=np.uint8)
    for r in range(h):
        for c in range(w):
            if reachable[get_id(r, c)]:
                mask[r, c] = 255
    
    return img, mask

# Usage example for one instance
if __name__ == "__main__":
    original, result = segment_image_scratch('hand.png')
    cv2.imshow("Original", original)
    cv2.imshow("Segmented Mask", result)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
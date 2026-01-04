import argparse
import sys
import os
import cv2
import numpy as np

# Increase recursion depth for deep DFS/BFS if needed, though we use iterative BFS.
sys.setrecursionlimit(10000)

class Edge:
    def __init__(self, to, capacity, flow=0, rev=None):
        self.to = to
        self.capacity = capacity
        self.flow = flow
        self.rev = rev  # Index of the reverse edge in adj[to]

class Graph:
    def __init__(self, num_nodes):
        self.num_nodes = num_nodes
        self.adj = [[] for _ in range(num_nodes)]

    def add_edge(self, u, v, capacity):
        # Forward edge
        forward = Edge(v, capacity)
        # Backward edge (residual)
        backward = Edge(u, 0) 
        
        # Store indices to find reverse edges quickly
        forward.rev = len(self.adj[v])
        backward.rev = len(self.adj[u])
        
        self.adj[u].append(forward)
        self.adj[v].append(backward)

def load_image(path, scale=1.0):
    img = cv2.imread(path)
    if img is None:
        raise ValueError(f"Could not load image from {path}")
    if scale != 1.0:
        width = int(img.shape[1] * scale)
        height = int(img.shape[0] * scale)
        img = cv2.resize(img, (width, height))
    return img

def build_graph(img):
    h, w = img.shape[:2]
    num_pixels = h * w
    # Nodes: 0 to num_pixels-1 are pixels. 
    # num_pixels is Source (S), num_pixels + 1 is Sink (T)
    S = num_pixels
    T = num_pixels + 1
    g = Graph(num_pixels + 2)
    
    # Parameters for capacity calculation
    # Beta calculation: 1 / (2 * <(Ip - Iq)^2>)
    # We need to iterate over all neighbors to calculate beta first
    diffs = []
    
    # Helper to get pixel index
    def get_idx(r, c):
        return r * w + c

    # Calculate intensity differences for beta
    # We'll use a simplified loop for clarity, though vectorization is faster
    # But since we need to build the graph edge by edge anyway, let's do it carefully.
    
    # Pre-calculate beta
    # Considering 4-connectivity: Right and Down for each pixel is enough to cover all edges once
    for r in range(h):
        for c in range(w):
            curr_intensity = img[r, c].astype(int)
            
            # Right neighbor
            if c + 1 < w:
                right_intensity = img[r, c+1].astype(int)
                sq_diff = np.sum((curr_intensity - right_intensity) ** 2)
                diffs.append(sq_diff)
            
            # Down neighbor
            if r + 1 < h:
                down_intensity = img[r+1, c].astype(int)
                sq_diff = np.sum((curr_intensity - down_intensity) ** 2)
                diffs.append(sq_diff)
                
    if not diffs:
        beta = 0
    else:
        beta = 1.0 / (2.0 * np.mean(diffs) + 1e-6) # Avoid division by zero
        
    # Capacity function
    def n_link_cap(diff_sq):
        return 50 * np.exp(-beta * diff_sq)

    # Build Graph
    # K is a large constant for hard constraints (seeds)
    K = 1e9 
    
    # Collect seeds for color model
    fg_pixels = []
    bg_pixels = []
    
    # First pass: Identify seeds and collect statistics
    # Relaxed border: 2% or at least 2 pixels
    border_margin = max(2, int(min(h, w) * 0.02))
    
    for r in range(h):
        for c in range(w):
            # BG Seeds: Very strict border
            if r < border_margin or r >= h - border_margin or c < border_margin or c >= w - border_margin:
                bg_pixels.append(img[r, c])
            
            # FG Seeds: Center box (keep 20% center)
            elif r > h * 0.4 and r < h * 0.6 and c > w * 0.4 and c < w * 0.6:
                fg_pixels.append(img[r, c])

    # Calculate Mean Colors
    if not fg_pixels:
        # Fallback if image is weird
        mean_fg = np.array([255, 255, 255]) 
    else:
        mean_fg = np.mean(fg_pixels, axis=0)
        
    if not bg_pixels:
        mean_bg = np.array([0, 0, 0])
    else:
        mean_bg = np.mean(bg_pixels, axis=0)
        
    # Simple sigma for color model (can be estimated too, but fixed is often stable enough)
    # Or use the same beta as N-links? Let's use a separate sigma.
    sigma_sq = 2 * np.var(img) + 1e-6 # Global variance as a rough scale
    
    # Lambda for data term weight relative to smoothness
    # If N-links are ~50, T-links should be comparable for uncertain regions.
    lambda_data = 10.0 

    for r in range(h):
        for c in range(w):
            u = get_idx(r, c)
            curr_intensity = img[r, c].astype(int)
            
            # --- N-links (Neighbor links) ---
            # Right
            if c + 1 < w:
                v = get_idx(r, c+1)
                right_intensity = img[r, c+1].astype(int)
                diff_sq = np.sum((curr_intensity - right_intensity) ** 2)
                cap = n_link_cap(diff_sq)
                g.add_edge(u, v, cap)
                g.add_edge(v, u, cap) # Undirected in terms of capacity, so add both ways
            
            # Down
            if r + 1 < h:
                v = get_idx(r+1, c)
                down_intensity = img[r+1, c].astype(int)
                diff_sq = np.sum((curr_intensity - down_intensity) ** 2)
                cap = n_link_cap(diff_sq)
                g.add_edge(u, v, cap)
                g.add_edge(v, u, cap)

            # --- T-links (Terminal links) ---
            is_bg_seed = (r < border_margin or r >= h - border_margin or c < border_margin or c >= w - border_margin)
            is_fg_seed = (r > h * 0.4 and r < h * 0.6 and c > w * 0.4 and c < w * 0.6)
            
            if is_fg_seed:
                g.add_edge(S, u, K)
                g.add_edge(u, T, 0)
            elif is_bg_seed:
                g.add_edge(S, u, 0)
                g.add_edge(u, T, K)
            else:
                # Data Term for uncertain pixels
                # Distances to models
                dist_fg = np.sum((curr_intensity - mean_fg) ** 2)
                dist_bg = np.sum((curr_intensity - mean_bg) ** 2)
                
                # Probabilities (unnormalized)
                # P(FG) ~ exp(-dist_fg / sigma)
                # P(BG) ~ exp(-dist_bg / sigma)
                
                # Capacities:
                # C(S, p) = Cost to cut S-p (assign to BG) -> Should be high if P(FG) is high.
                # Actually, standard formulation:
                # E_data(label) = -ln P(label)
                # If label=BG (cut S-p), cost is -ln P(BG).
                # If label=FG (cut p-T), cost is -ln P(FG).
                
                # Let's try simple inverse distance weighting if log is too sensitive
                # or just the negative log likelihood.
                
                # Using -ln(exp(-d/s)) = d/s
                cost_to_bg = dist_bg / sigma_sq # If dist_bg is small, cost is small -> likely BG
                cost_to_fg = dist_fg / sigma_sq # If dist_fg is small, cost is small -> likely FG
                
                # Wait, if I cut S-p, p becomes disconnected from S, so it belongs to T (BG).
                # So capacity of S-p should be the penalty for being BG.
                # Penalty for being BG should be HIGH if it looks like FG.
                # So C(S, p) should be proportional to P(FG) or inversely proportional to dist_fg.
                
                # Let's re-verify GraphCut logic.
                # Source = Object, Sink = Background.
                # Cut separates nodes into S-set (Object) and T-set (Background).
                # If edge (S, p) is cut, p is in T-set (Background).
                # Cost of cutting (S, p) should be the cost of assigning p to Background.
                # If p looks like Object, cost of assigning to Background should be HIGH.
                # So C(S, p) should be high if dist_fg is small.
                
                # Model: C(S, p) = lambda * -ln(P(BG)) ? No.
                # Let's use the standard GrabCut-like weights:
                # C(S, p) = -lambda * ln(P(Bkg))  <-- Penalty for being Bkg
                # C(p, T) = -lambda * ln(P(Obj))  <-- Penalty for being Obj
                
                # P(Bkg) ~ exp(-dist_bg / sigma)
                # -ln(P(Bkg)) ~ dist_bg / sigma
                
                # So:
                # C(S, p) = lambda * dist_bg
                # C(p, T) = lambda * dist_fg
                
                # Let's check:
                # If pixel is exactly FG color: dist_fg = 0, dist_bg = large.
                # C(S, p) = large. C(p, T) = 0.
                # Min cut will likely cut (p, T) (cost 0).
                # p remains connected to S. p is Object. CORRECT.
                
                # If pixel is exactly BG color: dist_fg = large, dist_bg = 0.
                # C(S, p) = 0. C(p, T) = large.
                # Min cut will likely cut (S, p) (cost 0).
                # p remains connected to T. p is Background. CORRECT.
                
                # Scaling factor
                # We need to normalize or scale this to match N-links (0-50 range).
                # dist_sq can be up to 255^2 * 3 ~ 195000.
                # sigma_sq is variance.
                
                # Let's use a simpler bounded function to avoid huge capacities dominating N-links
                # C = lambda * (1 - exp(-dist / sigma)) ? No, that's bounded 0-1.
                
                # Let's stick to the linear cost but scaled.
                # C(S, p) = lambda_data * (dist_bg / (dist_fg + dist_bg + 1e-6)) ?
                # This is a probability-like measure [0, lambda].
                # If dist_bg is large (unlike BG) and dist_fg is small (like FG), ratio -> 1. Cap -> lambda.
                # If dist_bg is small (like BG), ratio -> 0. Cap -> 0.
                
                # Let's try this normalized probability approach.
                # It keeps data terms in a predictable range relative to N-links.
                
                total_dist = dist_fg + dist_bg + 1e-6
                prob_fg = dist_bg / total_dist # If close to FG (dist_fg small), this is close to 1.
                prob_bg = dist_fg / total_dist
                
                # C(S, p) = Penalty for being BG = High if Prob(FG) is high.
                cap_s_p = lambda_data * prob_fg
                
                # C(p, T) = Penalty for being FG = High if Prob(BG) is high.
                cap_p_t = lambda_data * prob_bg
                
                g.add_edge(S, u, cap_s_p)
                g.add_edge(u, T, cap_p_t)

                
    return g, S, T

def bfs(graph, s, t, parent):
    fill_value = -1
    # Reset parent array
    for i in range(len(parent)):
        parent[i] = fill_value
        
    parent[s] = -2 # Mark source as visited
    queue = [(s, float('inf'))]
    
    while queue:
        u, flow = queue.pop(0)
        
        for edge in graph.adj[u]:
            v = edge.to
            if parent[v] == fill_value and edge.capacity - edge.flow > 0:
                parent[v] = edge.rev # Store index of reverse edge to backtrack
                new_flow = min(flow, edge.capacity - edge.flow)
                if v == t:
                    return new_flow
                queue.append((v, new_flow))
    return 0

def max_flow(graph, s, t):
    flow = 0
    parent = [-1] * graph.num_nodes
    
    while True:
        new_flow = bfs(graph, s, t, parent)
        if new_flow == 0:
            break
        
        flow += new_flow
        curr = t
        while curr != s:
            # parent[curr] stores the index of the reverse edge in adj[curr]
            # The reverse edge points back to the previous node (prev)
            # So graph.adj[curr][parent[curr]].to is the previous node
            
            rev_idx = parent[curr]
            rev_edge = graph.adj[curr][rev_idx]
            prev = rev_edge.to
            
            # The forward edge is the reverse of the reverse edge
            # rev_edge.rev is the index of the forward edge in adj[prev]
            fwd_idx = rev_edge.rev
            fwd_edge = graph.adj[prev][fwd_idx]
            
            fwd_edge.flow += new_flow
            rev_edge.flow -= new_flow
            
            curr = prev
            
    return flow

def find_min_cut(graph, s):
    # BFS from s in residual graph
    visited = [False] * graph.num_nodes
    queue = [s]
    visited[s] = True
    
    while queue:
        u = queue.pop(0)
        for edge in graph.adj[u]:
            # If there is residual capacity, we can reach v
            if not visited[edge.to] and edge.capacity - edge.flow > 0:
                visited[edge.to] = True
                queue.append(edge.to)
                
    return visited

def main():
    parser = argparse.ArgumentParser(description='Image Segmentation using Max-Flow Min-Cut')
    parser.add_argument('--input_path', help='Path to input image')
    parser.add_argument('--output_path', help='Path to save output image')
    
    args = parser.parse_args()
    
    if not os.path.exists(args.input_path):
        print(f"Error: Input file '{args.input_path}' not found.")
        sys.exit(1)
        
    print(f"Processing {args.input_path}...")
    print(f"Output will be saved to {args.output_path}")
    
    # 1. Load Image
    # For performance on large images, we might want to downscale, but let's try full res first
    # or a reasonable max dimension like 300px for speed in this assignment context.
    # The example images look small-ish.
    original_img = load_image(args.input_path)
    
    # Resize for processing if too large (optional, but good for graph algorithms in Python)
    # Let's keep it simple and use a fixed small size for the graph, then scale up the mask?
    # Or just process at a reasonable size e.g. max 100x100 for speed?
    # The assignment asks for "segmentation", usually pixel-wise.
    # Let's try a max dimension of 100 for speed, as Python max-flow is slow.
    max_dim = 100
    h, w = original_img.shape[:2]
    scale = 1.0
    if max(h, w) > max_dim:
        scale = max_dim / max(h, w)
        img_proc = cv2.resize(original_img, (int(w * scale), int(h * scale)))
    else:
        img_proc = original_img.copy()
        
    print(f"Image resized to {img_proc.shape[:2]} for processing.")

    # 2. Build Graph
    print("Building graph...")
    g, S, T = build_graph(img_proc)
    
    # 3. Max Flow
    print("Calculating max flow...")
    max_flow(g, S, T)
    
    # 4. Min Cut
    print("Extracting min cut...")
    visited = find_min_cut(g, S)
    
    # 5. Generate Mask
    h_proc, w_proc = img_proc.shape[:2]
    mask = np.zeros((h_proc, w_proc), dtype=np.uint8)
    
    for r in range(h_proc):
        for c in range(w_proc):
            idx = r * w_proc + c
            if visited[idx]: # Reachable from S -> Foreground
                mask[r, c] = 255
            else:
                mask[r, c] = 0
                
    # 6. Extract Contour
    # Resize mask back to original size
    mask_orig = cv2.resize(mask, (w, h), interpolation=cv2.INTER_NEAREST)
    
    # Find contours
    contours, _ = cv2.findContours(mask_orig, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # Draw contours on black background as per task requirement
    result = np.zeros_like(original_img)
    cv2.drawContours(result, contours, -1, (255, 255, 255), 2)
    
    # Save result
    cv2.imwrite(args.output_path, result)
        
    print("Done.")

if __name__ == '__main__':
    main()

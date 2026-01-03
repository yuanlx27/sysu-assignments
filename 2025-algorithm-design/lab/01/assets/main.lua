function AllocateBooks(pages, m)
	local function check(limit)
		local num, sum = 0, 0
		for i = 1, #pages do
			if sum + pages[i] > limit then
				num = num + 1
				sum = 0
			end
			sum = sum + pages[i]
		end
		if sum > 0 then
			num = num + 1
		end
		return num <= m
	end

	local l, r = 0, 2^31 - 1
	while l < r do
		local mid = math.floor((l + r) / 2)
		if check(mid) then
			r = mid
		else
			l = mid + 1
		end
	end

	print("The answer is " .. l)
end

local pages = {12, 34, 67, 90}
local m = 2
AllocateBooks(pages, m)

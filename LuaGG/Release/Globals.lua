TRUE = 1
FALSE = 0

MESSAGE_TYPE_FATAL_ERROR = 0
MESSAGE_TYPE_ERROR = 1
MESSAGE_TYPE_INFO = 2
MESSAGE_TYPE_NO_ICON = 3
MESSAGE_TYPE_YES_NO = 4

GG_STATUS_NOT_AVAILABLE = 0x0001
GG_STATUS_AVAILABLE = 0x0002
GG_STATUS_BUSY = 0x0003
GG_STATUS_AVAILABLE_DESCR = 0x0004
GG_STATUS_BUSY_DESCR = 0x0005
GG_STATUS_BLOCKED = 0x0006
GG_STATUS_INVISIBLE = 0x0014
GG_STATUS_NOT_AVAILABLE_DESCR = 0x0015
GG_STATUS_INVISIBLE_DESCR = 0x0016

INCLUDE_TEXT_IN_REGEX_PATTERN = 0
DONT_INCLUDE_TEXT_IN_REGEX_PATTERN = 1

function toNumberWithCommas(value)
    local newValue = value
    while true do  
        newValue, found = string.gsub(newValue, "^(-?%d+)(%d%d%d)", '%1,%2')
        if found == 0 then
			break
		end
    end
    return newValue
end

function string.explode(p, d, m)
	local limit = m or 0
	local t, ll, j
	t={}
	ll=0
	j=0
	if(#p == 1) then return p end
		while true do
			l = string.find(p, d, ll + 1, true) -- find the next d in the string
			if l ~= nil and (j < limit or limit == 0) then -- if "not not" found then..
				table.insert(t, string.sub(p, ll, l - 1)) -- Save it in our array.
				ll = l + 1 -- save just after where we found it for searching next time.
				j = j + 1 -- number of explosions
			else
				table.insert(t, string.sub(p, ll)) -- Save what's left in our array.
				break -- Break at end, as it should be, according to the lua manual.
			end
		end
	return t
end

function convertIntToIP(int, mask)
	local b4 = bit.urshift(bit.uband(int,  4278190080), 24)
	local b3 = bit.urshift(bit.uband(int,  16711680), 16)
	local b2 = bit.urshift(bit.uband(int,  65280), 8)
	local b1 = bit.urshift(bit.uband(int,  255), 0)
	if mask ~= nil then
		local m4 = bit.urshift(bit.uband(mask,  4278190080), 24)
		local m3 = bit.urshift(bit.uband(mask,  16711680), 16)
		local m2 = bit.urshift(bit.uband(mask,  65280), 8)
		local m1 = bit.urshift(bit.uband(mask,  255), 0)
		if (m1 == 255 or m1 == 0) and (m2 == 255 or m2 == 0) and (m3 == 255 or m3 == 0) and (m4 == 255 or m4 == 0) then
			if m1 == 0 then b1 = "x" end
			if m2 == 0 then b2 = "x" end
			if m3 == 0 then b3 = "x" end
			if m4 == 0 then b4 = "x" end
		else
			if m1 ~= 255 or m2 ~= 255 or m3 ~= 255 or m4 ~= 255 then
				return b1 .. "." .. b2 .. "." .. b3 .. "." .. b4 .. " : " .. m1 .. "." .. m2 .. "." .. m3 .. "." .. m4
			end
		end
	end

	return b1 .. "." .. b2 .. "." .. b3 .. "." .. b4
end

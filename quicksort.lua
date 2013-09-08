function quicksort(L)
    if #L == 1 then
        return L
    end

    local pivot = table.remove(L, #L / 2)
    local l = {}
    local r = {}

    if pivot == nil then return L end

    for _, el in ipairs(L) do
        if el < pivot then
            table.insert(l, el)
        else
            table.insert(r, el)
        end
    end
   
    table.insert(l, pivot)
    local rl = quicksort(l)
    local rr = quicksort(r)

    for _, v in ipairs(rr) do
        table.insert(rl, v)
    end

    return rl
end


local array = {9, 8, 7, 3, 2, 1, 100, 2, 55, 33, 0, 13}
print(unpack(quicksort(array)))

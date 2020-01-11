local card = {}

function card.test_text(self)
	return card.properties.text == "ok"
end

function card.test_number(self)
	return card.properties.number == 1
end

function card.test_boolean(self)
	return card.properties.boolean == true -- needed because it might be nil
end

return card
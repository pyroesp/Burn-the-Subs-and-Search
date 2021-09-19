SELECT id, userName, positionX, positionY, fontSize, entryTime FROM sub
WHERE id = %d
ORDER BY entryTime DESC
LIMIT 20;
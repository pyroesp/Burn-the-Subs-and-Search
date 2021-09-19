SELECT id, userName, positionX, positionY, fontSize, entryTime FROM sub
WHERE userName = '%s'
ORDER BY entryTime DESC
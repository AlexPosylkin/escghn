$scale 2.0
$modelname	"escghn/escghn_turretguns.mdl"
$staticprop
$body escghn_turretguns "escghn_turretguns.smd"
$collisionmodel "escghn_turretguns_hull.smd" {
	$concave
	$maxconvexpieces 76
	$automass

}
$surfaceprop "metal" 
$cdmaterials "escghn"
$sequence idle "escghn_turretguns.smd" activity ACT_IDLE -1 fps 1
$attachment "muzzle" "Bone001" -36.9833 -41.4213 45.8586 rotate 0 -89.9998 0
$attachment "muzzle1" "Bone001" 37.0901 -41.4275 45.8531 rotate 0 -89.9998 0
$attachment "muzzle2" "Bone001" 30.0883 -60.0397 64.6872 rotate 0 -89.9998 0
$attachment "muzzle3" "Bone001" -29.9239 -60.0481 64.6891 rotate 0 -89.9998 0


$scale 2.0
$modelname	"escghn/escghn_turretbase.mdl"
$staticprop
$body escghn_turretbase "escghn_turretbase.smd"
$collisionmodel "escghn_turretbase_hull.smd" {
	$concave
	$maxconvexpieces 24

}
$surfaceprop "metal" 
$contents "solid"
$cdmaterials "escghn"
$sequence idle "escghn_turretbase.smd" activity ACT_IDLE -1 fps 1

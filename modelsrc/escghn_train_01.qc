$scale 1.2
$modelname	"escghn/escghn_train_01.mdl"
$staticprop
$body escghn_train_01 "escghn_train_01.smd"
$collisionmodel "escghn_train_01_hull.smd" {
	$concave
	$maxconvexpieces 65
	$automass

}
$surfaceprop "metal" 
$cdmaterials "escghn"
$sequence idle "escghn_train_01.smd" activity ACT_IDLE -1 fps 1

$include	"escghn_train_01_wwmt_custom.qci"

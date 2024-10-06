extends Node2D

var frequency = 440

func  _process(delta: float) -> void:
	var m_pos = get_global_mouse_position()
	frequency = 1000 - m_pos.y
	$GDJucy.set_frequency(frequency)			# pass parameters to GDExtension GDJucy
	queue_redraw()

func _draw() -> void:
	# Drawing Sine Wave ----------------------
	var color = Color(0.5, 1.0, 0.8, 0.6)
	var array = PackedVector2Array()
	for i in range(1000):
		var y = 300*sin(i*2*PI/50000.0*frequency) + 320
		var vec = Vector2(i, y)
		array.append(vec)

	draw_polyline(array, color)

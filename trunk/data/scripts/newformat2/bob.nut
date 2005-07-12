add_question("Do you like Frank?");
add_answers("Yes.", "No.");
switch (get_answer())
{
case 0:
	like_frank = true;
	break;
case 1:
	like_frank = false;
	break;
}
end_dialog();
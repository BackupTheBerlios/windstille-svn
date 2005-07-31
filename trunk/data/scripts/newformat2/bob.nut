function intro()
{
	bob_seen_before = true;
	dialog(0, "images/portrait.sprite", "Hey, my name's bob.  Are you new around here?"); 
	conversation_add("Yes, I am.");
	conversation_add("Go away"); 
	if (!conversation_get())
	{
      ask_questions();
	}
	else
	{
      bye();
	}
}

function hello_again()
{
	dialog(0, "images/portrait.sprite", "Hello again");
	conversation_add("Hey.");
	conversation_add("Bye");
	if (!conversation_get())
		ask_questions();
	else
		bye();
}

function ask_questions()
{
	dialog(0, "images/portrait.sprite", "So what are you doing here?");
	conversation_add("I work as a mercenary.");
	conversation_add("Not much");
	if (!conversation_get())
	{
      bob_knows_your_mercenary = true;
      offer_job();
	}
	else
		bye();
}

function offer_job()
{
	dialog(0, "images/portrait.sprite", "Are you looking for some work?");
	conversation_add("I might be, if the offer is good enough.");
	conversation_add("No thanks.");
	if (!conversation_get())
	{
      bob_knows_job = true;
      explain_job();
	}
	else
		bye();
}

function explain_job()
{
	dialog(0, "images/portrait.sprite", "Your job is to kill the spider at the end of the level ;)");
}

function bye()
{
	if (bob_knows_job)
		dialog(0, "images/portrait.sprite", "Feel free to come back if you need a job");
	else
		dialog(0, "images/portrait.sprite", "See you again some time");
}

//sintro();

if (bob_knows_job)
	explain_job();
else if (bob_knows_your_mercenary)
	offer_job();
else if (bob_seen_before)
	hello_again();
else
   intro();

/* EOF */

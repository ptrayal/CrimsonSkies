/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.    *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  ROM 2.4 improvements copyright (C) 1993-1998 Russ Taylor, Gabrielle    *
 *  Taylor and Brian Moore                                                 *
 *                                                                         *
 *  Crimson Skies (CS-Mud) copyright (C) 1998-2015 by Blake Pell           *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt' as well as the ROM license.  In particular,   *
 *  you may not remove these copyright notices.                            *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

// System Specific Includes
#if defined(__APPLE__)
    #include <types.h>
#elif defined(_WIN32)
    #include <sys/types.h>
    #include <time.h>
#else
    #include <sys/types.h>
    #include <sys/time.h>
    #include <time.h>
#endif

// General Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include <ctype.h> /* for isalpha() and isspace() -- JR */

/* RT code to delete yourself */

/*
 * Void to force player to fully type out delete if they want to delete.
 */
void do_delet (CHAR_DATA * ch, char *argument)
{
    send_to_char ("You must type the full command to delete yourself.\n\r", ch);
} // end void do_delet

/*
 * Delete command that will allow a player to delete their pfile.
 */
void do_delete (CHAR_DATA * ch, char *argument)
{
    char strsave[MAX_INPUT_LENGTH];

    if (IS_NPC (ch))
        return;

    if (ch->pcdata->confirm_delete)
    {
        if (argument[0] != '\0')
        {
            send_to_char ("Delete status removed.\n\r", ch);
            ch->pcdata->confirm_delete = FALSE;
            return;
        }
        else
        {
            sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->name));
            wiznet ("$N turns $Mself into line noise.", ch, NULL, 0, 0, 0);
            stop_fighting (ch, TRUE);
            do_function (ch, &do_quit, "");

#if defined(_WIN32)
            _unlink(strsave);
#else
            unlink (strsave);
#endif

            return;
        }
    }

    if (argument[0] != '\0')
    {
        send_to_char ("Just type delete. No argument.\n\r", ch);
        return;
    }

    send_to_char ("Type delete again to confirm this command.\n\r", ch);
    send_to_char ("WARNING: this command is irreversible.\n\r", ch);
    send_to_char ("Typing delete with an argument will undo delete status.\n\r", ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet ("$N is contemplating deletion.", ch, NULL, 0, 0, get_trust (ch));
} // end void do_delete

/*
 * RT code to display channel status
 */
void do_channels (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    send_to_char ("Channel        Status\n\r", ch);
    send_to_char ("---------------------\n\r", ch);

    send_to_char ("{dgossip{x         ", ch);
    if (!IS_SET (ch->comm, COMM_NOGOSSIP))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    send_to_char ("{aauction{x        ", ch);
    if (!IS_SET (ch->comm, COMM_NOAUCTION))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    send_to_char ("{qQ{x/{fA{x            ", ch);
    if (!IS_SET (ch->comm, COMM_NOQUESTION))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    send_to_char ("{tgrats{x          ", ch);
    if (!IS_SET (ch->comm, COMM_NOGRATS))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    if (IS_IMMORTAL (ch))
    {
        send_to_char ("{igod channel{x    ", ch);
        if (!IS_SET (ch->comm, COMM_NOWIZ))
            send_to_char ("{GON{x\n\r", ch);
        else
            send_to_char ("{ROFF{x\n\r", ch);
    }

    send_to_char ("{tshouts{x         ", ch);
    if (!IS_SET (ch->comm, COMM_SHOUTSOFF))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    send_to_char ("{ktells{x          ", ch);
    if (!IS_SET (ch->comm, COMM_DEAF))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    send_to_char ("{tquiet mode{x     ", ch);
    if (IS_SET (ch->comm, COMM_QUIET))
        send_to_char ("{GON{x\n\r", ch);
    else
        send_to_char ("{ROFF{x\n\r", ch);

    if (IS_SET (ch->comm, COMM_AFK))
        send_to_char ("You are AFK.\n\r", ch);

    if (IS_SET (ch->comm, COMM_SNOOP_PROOF))
        send_to_char ("You are immune to snooping.\n\r", ch);

    if (ch->lines != PAGELEN)
    {
        if (ch->lines)
        {
            sprintf (buf, "You display %d lines of scroll.\n\r",
                     ch->lines + 2);
            send_to_char (buf, ch);
        }
        else
            send_to_char ("Scroll buffering is off.\n\r", ch);
    }

    if (ch->prompt != NULL)
    {
        sprintf (buf, "Your current prompt is: %s\n\r", ch->prompt);
        send_to_char (buf, ch);
    }

    if (IS_SET (ch->comm, COMM_NOSHOUT))
        send_to_char ("You cannot shout.\n\r", ch);

    if (IS_SET (ch->comm, COMM_NOTELL))
        send_to_char ("You cannot use tell.\n\r", ch);

    if (IS_SET (ch->comm, COMM_NOCHANNELS))
        send_to_char ("You cannot use channels.\n\r", ch);

    if (IS_SET (ch->comm, COMM_NOEMOTE))
        send_to_char ("You cannot show emotions.\n\r", ch);

} // end void do_channels

/*
 * RT deaf blocks out all shouts and a few other forms of communication.
 */
void do_deaf (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_DEAF))
    {
        send_to_char ("You can now hear tells again.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_DEAF);
    }
    else
    {
        send_to_char ("From now on, you won't hear tells.\n\r", ch);
        SET_BIT (ch->comm, COMM_DEAF);
    }
} // end do_deaf

/*
 * RT quiet blocks out all communication
 */
void do_quiet (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_QUIET))
    {
        send_to_char ("Quiet mode removed.\n\r", ch);
        REMOVE_BIT (ch->comm, COMM_QUIET);
    }
    else
    {
        send_to_char ("From now on, you will only hear says and emotes.\n\r",
                      ch);
        SET_BIT (ch->comm, COMM_QUIET);
    }
} // end do_quiet

/*
 * AFK (away from keyboard) command
 */
void do_afk (CHAR_DATA * ch, char *argument)
{
    if (IS_SET (ch->comm, COMM_AFK))
    {
        send_to_char ("AFK mode removed. Type 'replay' to see tells.\n\r",
                      ch);
        REMOVE_BIT (ch->comm, COMM_AFK);
    }
    else
    {
        send_to_char ("You are now in AFK mode.\n\r", ch);
        SET_BIT (ch->comm, COMM_AFK);
    }
} // end do_afk

/*
 * Replay command, attempts to show the users tells that they missed while they
 * were AFK or in a mode where they could not see them.
 */
void do_replay (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
    {
        send_to_char ("You can't replay.\n\r", ch);
        return;
    }

    if (buf_string (ch->pcdata->buffer)[0] == '\0')
    {
        send_to_char ("You have no tells to replay.\n\r", ch);
        return;
    }

    page_to_char (buf_string (ch->pcdata->buffer), ch);
    clear_buf (ch->pcdata->buffer);
} // end do_replay

/*
 * RT auction channel rewritten in ROM style
 */
void do_auction (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOAUCTION))
        {
            send_to_char ("{aAuction channel is now ON.{x\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOAUCTION);
        }
        else
        {
            send_to_char ("{aAuction channel is now OFF.{x\n\r", ch);
            SET_BIT (ch->comm, COMM_NOAUCTION);
        }
    }
    else
    {                            /* auction message sent, turn auction on if it is off */

        if (IS_SET (ch->comm, COMM_QUIET))
        {
            send_to_char ("You must turn off quiet mode first.\n\r", ch);
            return;
        }

        if (IS_SET (ch->comm, COMM_NOCHANNELS))
        {
            send_to_char
                ("The gods have revoked your channel priviliges.\n\r", ch);
            return;
        }

        REMOVE_BIT (ch->comm, COMM_NOAUCTION);
    }

    sprintf (buf, "{xYou auction '{m%s{x'\n\r", argument);
    send_to_char (buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if (d->connected == CON_PLAYING &&
            d->character != ch &&
            !IS_SET (victim->comm, COMM_NOAUCTION) &&
            !IS_SET (victim->comm, COMM_QUIET))
        {
            act_new ("{x$n auctions '{m$t{x'",
                     ch, argument, d->character, TO_VICT, POS_DEAD);
        }
    }
} // end do_auction

/* RT chat replaced with ROM gossip */
void do_gossip (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOGOSSIP))
        {
            send_to_char ("Gossip channel is now ON.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOGOSSIP);
        }
        else
        {
            send_to_char ("Gossip channel is now OFF.\n\r", ch);
            SET_BIT (ch->comm, COMM_NOGOSSIP);
        }
    }
    else
    {                            /* gossip message sent, turn gossip on if it isn't already */

        if (IS_SET (ch->comm, COMM_QUIET))
        {
            send_to_char ("You must turn off quiet mode first.\n\r", ch);
            return;
        }

        if (IS_SET (ch->comm, COMM_NOCHANNELS))
        {
            send_to_char
                ("The gods have revoked your channel priviliges.\n\r", ch);
            return;

        }

        REMOVE_BIT (ch->comm, COMM_NOGOSSIP);

        sprintf (buf, "{xYou gossip '{w%s{x'\n\r", argument);
        send_to_char (buf, ch);
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if (d->connected == CON_PLAYING &&
                d->character != ch &&
                !IS_SET (victim->comm, COMM_NOGOSSIP) &&
                !IS_SET (victim->comm, COMM_QUIET))
            {
                act_new ("{x$n gossips '{w$t{x'",
                         ch, argument, d->character, TO_VICT, POS_SLEEPING);
            }
        }
    }
}

void do_grats (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOGRATS))
        {
            send_to_char ("Grats channel is now ON.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOGRATS);
        }
        else
        {
            send_to_char ("Grats channel is now OFF.\n\r", ch);
            SET_BIT (ch->comm, COMM_NOGRATS);
        }
    }
    else
    {                            /* grats message sent, turn grats on if it isn't already */

        if (IS_SET (ch->comm, COMM_QUIET))
        {
            send_to_char ("You must turn off quiet mode first.\n\r", ch);
            return;
        }

        if (IS_SET (ch->comm, COMM_NOCHANNELS))
        {
            send_to_char
                ("The gods have revoked your channel priviliges.\n\r", ch);
            return;

        }

        REMOVE_BIT (ch->comm, COMM_NOGRATS);

        sprintf (buf, "{xYou grats '{c%s{x'\n\r", argument);
        send_to_char (buf, ch);
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if (d->connected == CON_PLAYING &&
                d->character != ch &&
                !IS_SET (victim->comm, COMM_NOGRATS) &&
                !IS_SET (victim->comm, COMM_QUIET))
            {
                act_new ("{x$n grats '{c$t{x'",
                         ch, argument, d->character, TO_VICT, POS_SLEEPING);
            }
        }
    }
}

/* RT question channel */
void do_question (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOQUESTION))
        {
            send_to_char ("Q/A channel is now ON.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOQUESTION);
        }
        else
        {
            send_to_char ("Q/A channel is now OFF.\n\r", ch);
            SET_BIT (ch->comm, COMM_NOQUESTION);
        }
    }
    else
    {                            /* question sent, turn Q/A on if it isn't already */

        if (IS_SET (ch->comm, COMM_QUIET))
        {
            send_to_char ("You must turn off quiet mode first.\n\r", ch);
            return;
        }

        if (IS_SET (ch->comm, COMM_NOCHANNELS))
        {
            send_to_char
                ("The gods have revoked your channel priviliges.\n\r", ch);
            return;
        }

        REMOVE_BIT (ch->comm, COMM_NOQUESTION);

        sprintf (buf, "{xYou question '{Y%s{x'\n\r", argument);
        send_to_char (buf, ch);
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if (d->connected == CON_PLAYING &&
                d->character != ch &&
                !IS_SET (victim->comm, COMM_NOQUESTION) &&
                !IS_SET (victim->comm, COMM_QUIET))
            {
                act_new ("{x$n questions '{Y$t{x'",
                         ch, argument, d->character, TO_VICT, POS_SLEEPING);
            }
        }
    }
}

/* RT answer channel - uses same line as questions */
void do_answer (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOQUESTION))
        {
            send_to_char ("Q/A channel is now ON.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOQUESTION);
        }
        else
        {
            send_to_char ("Q/A channel is now OFF.\n\r", ch);
            SET_BIT (ch->comm, COMM_NOQUESTION);
        }
    }
    else
    {                            /* answer sent, turn Q/A on if it isn't already */

        if (IS_SET (ch->comm, COMM_QUIET))
        {
            send_to_char ("You must turn off quiet mode first.\n\r", ch);
            return;
        }

        if (IS_SET (ch->comm, COMM_NOCHANNELS))
        {
            send_to_char
                ("The gods have revoked your channel priviliges.\n\r", ch);
            return;
        }

        REMOVE_BIT (ch->comm, COMM_NOQUESTION);

        sprintf (buf, "{xYou answer '{Y%s{x'\n\r", argument);
        send_to_char (buf, ch);
        for (d = descriptor_list; d != NULL; d = d->next)
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if (d->connected == CON_PLAYING &&
                d->character != ch &&
                !IS_SET (victim->comm, COMM_NOQUESTION) &&
                !IS_SET (victim->comm, COMM_QUIET))
            {
                act_new ("{x$n answers '{Y$t{x'",
                         ch, argument, d->character, TO_VICT, POS_SLEEPING);
            }
        }
    }
}

/* clan channels */
void do_clantalk (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!is_clan (ch) || clan_table[ch->clan].independent)
    {
        send_to_char ("You aren't in a clan.\n\r", ch);
        return;
    }
    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOCLAN))
        {
            send_to_char ("Clan channel is now ON\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOCLAN);
        }
        else
        {
            send_to_char ("Clan channel is now OFF\n\r", ch);
            SET_BIT (ch->comm, COMM_NOCLAN);
        }
        return;
    }

    if (IS_SET (ch->comm, COMM_NOCHANNELS))
    {
        send_to_char ("The gods have revoked your channel priviliges.\n\r",
                      ch);
        return;
    }

    REMOVE_BIT (ch->comm, COMM_NOCLAN);

    sprintf (buf, "{xYou clan '{G%s{x'\n\r", argument);
    send_to_char (buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING &&
            d->character != ch &&
            is_same_clan (ch, d->character) &&
            !IS_SET (d->character->comm, COMM_NOCLAN) &&
            !IS_SET (d->character->comm, COMM_QUIET))
        {
            act_new ("{x$n clans '{G$t{x'", ch, argument, d->character, TO_VICT,
                     POS_DEAD);
        }
    }

    return;
}

void do_immtalk (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_NOWIZ))
        {
            send_to_char ("Immortal channel is now ON\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_NOWIZ);
        }
        else
        {
            send_to_char ("Immortal channel is now OFF\n\r", ch);
            SET_BIT (ch->comm, COMM_NOWIZ);
        }
        return;
    }

    REMOVE_BIT (ch->comm, COMM_NOWIZ);

    act_new ("{c[{Y$n{c]: {W$t{x", ch, argument, NULL, TO_CHAR, POS_DEAD);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING &&
            IS_IMMORTAL (d->character) &&
            !IS_SET (d->character->comm, COMM_NOWIZ))
        {
            act_new ("{c[{Y$n{c]: {W$t{x", ch, argument, d->character, TO_VICT,
                     POS_DEAD);
        }
    }

    return;
}

/*
 * Rhien, 5/16/2015
 * Prayer channel, a direct line from a mortal to the immortals online.
 */
void do_pray(CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;

	if (argument[0] == '\0')
	{
        send_to_char("What do you wish to pray?\n\r", ch);
		return;
	}
	else
	{                            
		sprintf(buf, "{xYou pray '{G%s{x'\n\r", argument);
		send_to_char(buf, ch);
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			CHAR_DATA *victim;

			victim = d->original ? d->original : d->character;

			if (d->connected == CON_PLAYING &&
				d->character != ch &&
				IS_IMMORTAL(d->character) &&
				!IS_SET(victim->comm, COMM_QUIET))
			{
				act_new("{x$n prays '{G$t{x'",
					ch, argument, d->character, TO_VICT, POS_SLEEPING);
			}
		}
	}
} // end do_pray

void do_say (CHAR_DATA * ch, char *argument)
{
    if (argument[0] == '\0')
    {
        send_to_char ("Say what?\n\r", ch);
        return;
    }

    act ("{x$n says '{g$T{x'", ch, NULL, argument, TO_ROOM);
    act ("{xYou say '{g$T{x'", ch, NULL, argument, TO_CHAR);

    if (!IS_NPC (ch))
    {
        CHAR_DATA *mob, *mob_next;
        for (mob = ch->in_room->people; mob != NULL; mob = mob_next)
        {
            mob_next = mob->next_in_room;
            if (IS_NPC (mob) && HAS_TRIGGER (mob, TRIG_SPEECH)
                && mob->position == mob->pIndexData->default_pos)
                mp_act_trigger (argument, mob, ch, NULL, NULL, TRIG_SPEECH);
        }
    }
    return;
}



void do_shout (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
        if (IS_SET (ch->comm, COMM_SHOUTSOFF))
        {
            send_to_char ("You can hear shouts again.\n\r", ch);
            REMOVE_BIT (ch->comm, COMM_SHOUTSOFF);
        }
        else
        {
            send_to_char ("You will no longer hear shouts.\n\r", ch);
            SET_BIT (ch->comm, COMM_SHOUTSOFF);
        }
        return;
    }

    if (IS_SET (ch->comm, COMM_NOSHOUT))
    {
        send_to_char ("You can't shout.\n\r", ch);
        return;
    }

    REMOVE_BIT (ch->comm, COMM_SHOUTSOFF);

    WAIT_STATE (ch, 12);

    act ("{xYou shout '{Y$T{x'", ch, NULL, argument, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if (d->connected == CON_PLAYING &&
            d->character != ch &&
            !IS_SET (victim->comm, COMM_SHOUTSOFF) &&
            !IS_SET (victim->comm, COMM_QUIET))
        {
            act ("{x$n shouts '{Y$t{x'", ch, argument, d->character, TO_VICT);
        }
    }

    return;
}

void do_tell (CHAR_DATA * ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_SET (ch->comm, COMM_NOTELL) || IS_SET (ch->comm, COMM_DEAF))
    {
        send_to_char ("Your message didn't get through.\n\r", ch);
        return;
    }

    if (IS_SET (ch->comm, COMM_QUIET))
    {
        send_to_char ("You must turn off quiet mode first.\n\r", ch);
        return;
    }

    if (IS_SET (ch->comm, COMM_DEAF))
    {
        send_to_char ("You must turn off deaf mode first.\n\r", ch);
        return;
    }

    argument = one_argument (argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        send_to_char ("Tell whom what?\n\r", ch);
        return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ((victim = get_char_world (ch, arg)) == NULL
        || (IS_NPC (victim) && victim->in_room != ch->in_room))
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->desc == NULL && !IS_NPC (victim))
    {
        act ("$N seems to have misplaced $S link...try again later.",
             ch, NULL, victim, TO_CHAR);
        sprintf (buf, "{x%s tells you '{W%s{x'{x\n\r", PERS (ch, victim),
                 argument);
        buf[0] = UPPER (buf[0]);
        add_buf (victim->pcdata->buffer, buf);
        return;
    }

    if (!(IS_IMMORTAL (ch) && ch->level > LEVEL_IMMORTAL)
        && !IS_AWAKE (victim))
    {
        act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
        return;
    }

    if (
        (IS_SET (victim->comm, COMM_QUIET)
         || IS_SET (victim->comm, COMM_DEAF)) && !IS_IMMORTAL (ch))
    {
        act ("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
        return;
    }

    if (IS_SET (victim->comm, COMM_AFK))
    {
        if (IS_NPC (victim))
        {
            act ("$E is AFK, and not receiving tells.", ch, NULL, victim,
                 TO_CHAR);
            return;
        }

        act ("$E is AFK, but your tell will go through when $E returns.",
             ch, NULL, victim, TO_CHAR);
        sprintf (buf, "{x%s tells you '{W%s{x'\n\r", PERS (ch, victim),
                 argument);
        buf[0] = UPPER (buf[0]);
        add_buf (victim->pcdata->buffer, buf);
        return;
    }

    act ("{xYou tell $N '{W$t{x'", ch, argument, victim, TO_CHAR);
    act_new ("{x$n tells you '{W$t{x'", ch, argument, victim, TO_VICT,
             POS_DEAD);
    victim->reply = ch;

    if (!IS_NPC (ch) && IS_NPC (victim) && HAS_TRIGGER (victim, TRIG_SPEECH))
        mp_act_trigger (argument, victim, ch, NULL, NULL, TRIG_SPEECH);

    return;
}


void do_reply (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    if (IS_SET (ch->comm, COMM_NOTELL))
    {
        send_to_char ("Your message didn't get through.\n\r", ch);
        return;
    }

    if ((victim = ch->reply) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (victim->desc == NULL && !IS_NPC (victim))
    {
        act ("$N seems to have misplaced $S link...try again later.",
             ch, NULL, victim, TO_CHAR);
        sprintf (buf, "{x%s tells you '{W%s{x'\n\r", PERS (ch, victim),
                 argument);
        buf[0] = UPPER (buf[0]);
        add_buf (victim->pcdata->buffer, buf);
        return;
    }

    if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
    {
        act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
        return;
    }

    if (
        (IS_SET (victim->comm, COMM_QUIET)
         || IS_SET (victim->comm, COMM_DEAF)) && !IS_IMMORTAL (ch)
        && !IS_IMMORTAL (victim))
    {
        act_new ("$E is not receiving tells.", ch, 0, victim, TO_CHAR,
                 POS_DEAD);
        return;
    }

    if (!IS_IMMORTAL (victim) && !IS_AWAKE (ch))
    {
        send_to_char ("In your dreams, or what?\n\r", ch);
        return;
    }

    if (IS_SET (victim->comm, COMM_AFK))
    {
        if (IS_NPC (victim))
        {
            act_new ("$E is AFK, and not receiving tells.",
                     ch, NULL, victim, TO_CHAR, POS_DEAD);
            return;
        }

        act_new ("$E is AFK, but your tell will go through when $E returns.",
                 ch, NULL, victim, TO_CHAR, POS_DEAD);
        sprintf (buf, "{x%s tells you '{W%s{x'\n\r", PERS (ch, victim),
                 argument);
        buf[0] = UPPER (buf[0]);
        add_buf (victim->pcdata->buffer, buf);
        return;
    }

    act_new ("{xYou tell $N '{W$t{x'", ch, argument, victim, TO_CHAR,
             POS_DEAD);
    act_new ("{x$n tells you '{W$t{x'", ch, argument, victim, TO_VICT,
             POS_DEAD);
    victim->reply = ch;

    return;
}

void do_yell (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (IS_SET (ch->comm, COMM_NOSHOUT))
    {
        send_to_char ("You can't yell.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char ("Yell what?\n\r", ch);
        return;
    }


    act ("{xYou yell '{Y$t{x'", ch, argument, NULL, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->connected == CON_PLAYING
            && d->character != ch
            && d->character->in_room != NULL
            && d->character->in_room->area == ch->in_room->area
            && !IS_SET (d->character->comm, COMM_QUIET))
        {
            act ("{x$n yells '{Y$t{x'", ch, argument, d->character, TO_VICT);
        }
    }

    return;
}

void do_emote (CHAR_DATA * ch, char *argument)
{
    if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
        send_to_char ("You can't show your emotions.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char ("Emote what?\n\r", ch);
        return;
    }

    /* little hack to fix the ',{' bug posted to rom list
     * around 4/16/01 -- JR
     */
    if (!(isalpha(argument[0])) || (isspace(argument[0])))
    {
    send_to_char ("Moron!\n\r", ch);
    return;
    }

    MOBtrigger = FALSE;
    act ("{x$n $T", ch, NULL, argument, TO_ROOM);
    act ("{x$n $T", ch, NULL, argument, TO_CHAR);
    MOBtrigger = TRUE;
    return;
}


void do_pmote (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *vch;
    char *letter, *name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if (!IS_NPC (ch) && IS_SET (ch->comm, COMM_NOEMOTE))
    {
        send_to_char ("You can't show your emotions.\n\r", ch);
        return;
    }

    if (argument[0] == '\0')
    {
        send_to_char ("Emote what?\n\r", ch);
        return;
    }

    /* little hack to fix the ',{' bug posted to rom list
     * around 4/16/01 -- JR
     */
    if (!(isalpha(argument[0])) || (isspace(argument[0])))
    {
    send_to_char ("Moron!\n\r", ch);
    return;
    }

    act ("{x$n $t", ch, argument, NULL, TO_CHAR);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;

        if ((letter = strstr (argument, vch->name)) == NULL)
        {
            MOBtrigger = FALSE;
            act ("{x$N $t", vch, argument, ch, TO_CHAR);
            MOBtrigger = TRUE;
            continue;
        }

        strcpy (temp, argument);
        temp[strlen (argument) - strlen (letter)] = '\0';
        last[0] = '\0';
        name = vch->name;

        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == strlen (vch->name))
            {
                strcat (temp, "r");
                continue;
            }

            if (*letter == 's' && matches == strlen (vch->name))
            {
                matches = 0;
                continue;
            }

            if (matches == strlen (vch->name))
            {
                matches = 0;
            }

            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == strlen (vch->name))
                {
                    strcat (temp, "you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat (last, letter, 1);
                continue;
            }

            matches = 0;
            strcat (temp, last);
            strncat (temp, letter, 1);
            last[0] = '\0';
            name = vch->name;
        }

        MOBtrigger = FALSE;
        act ("{x$N $t", vch, temp, ch, TO_CHAR);
        MOBtrigger = TRUE;
    }

    return;
}

void do_bug (CHAR_DATA * ch, char *argument)
{
    append_file (ch, BUG_FILE, argument);
    send_to_char ("Bug logged.\n\r", ch);
    return;
}

void do_typo (CHAR_DATA * ch, char *argument)
{
    append_file (ch, TYPO_FILE, argument);
    send_to_char ("Typo logged.\n\r", ch);
    return;
}

void do_qui (CHAR_DATA * ch, char *argument)
{
    send_to_char ("If you want to QUIT, you have to spell it out.\n\r", ch);
    return;
}

void do_quit (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d, *d_next;
    int id;

    if (IS_NPC (ch))
        return;

    if (ch->position == POS_FIGHTING)
    {
        send_to_char ("No way! You are fighting.\n\r", ch);
        return;
    }

    if (ch->position < POS_STUNNED)
    {
        send_to_char ("You're not DEAD yet.\n\r", ch);
        return;
    }
    send_to_char ("Alas, all good things must come to an end.\n\r", ch);
    act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);
    sprintf (log_buf, "%s has quit.", ch->name);
    log_string (log_buf);
    wiznet ("$N rejoins the real world.", ch, NULL, WIZ_LOGINS, 0,
            get_trust (ch));

    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj (ch);

    id = ch->id;
    d = ch->desc;
    extract_char (ch, TRUE);
    if (d != NULL)
        close_socket (d);

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
        CHAR_DATA *tch;

        d_next = d->next;
        tch = d->original ? d->original : d->character;
        if (tch && tch->id == id)
        {
            extract_char (tch, TRUE);
            close_socket (d);
        }
    }

    return;
}



void do_save (CHAR_DATA * ch, char *argument)
{
    if (IS_NPC (ch))
        return;

    save_char_obj (ch);
    send_to_char("Saving. Remember that Crimson Skies has automatic saving now.\n\r", ch);

    // Add lag on save, but on if it's not an immortal
    if (!IS_IMMORTAL(ch)) {
        WAIT_STATE(ch, 4 * PULSE_VIOLENCE);
    }

    return;
}



void do_follow (CHAR_DATA * ch, char *argument)
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        send_to_char ("Follow whom?\n\r", ch);
        return;
    }

    if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL)
    {
        act ("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
        return;
    }

    if (victim == ch)
    {
        if (ch->master == NULL)
        {
            send_to_char ("You already follow yourself.\n\r", ch);
            return;
        }
        stop_follower (ch);
        return;
    }

    if (!IS_NPC (victim) && IS_SET (victim->act, PLR_NOFOLLOW)
        && !IS_IMMORTAL (ch))
    {
        act ("$N doesn't seem to want any followers.\n\r", ch, NULL, victim,
             TO_CHAR);
        return;
    }

    REMOVE_BIT (ch->act, PLR_NOFOLLOW);

    if (ch->master != NULL)
        stop_follower (ch);

    add_follower (ch, victim);
    return;
}


void add_follower (CHAR_DATA * ch, CHAR_DATA * master)
{
    if (ch->master != NULL)
    {
        bug ("Add_follower: non-null master.", 0);
        return;
    }

    ch->master = master;
    ch->leader = NULL;

    if (can_see (master, ch))
        act ("$n now follows you.", ch, NULL, master, TO_VICT);

    act ("You now follow $N.", ch, NULL, master, TO_CHAR);

    return;
}



void stop_follower (CHAR_DATA * ch)
{
    if (ch->master == NULL)
    {
        bug ("Stop_follower: null master.", 0);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM))
    {
        REMOVE_BIT (ch->affected_by, AFF_CHARM);
        affect_strip (ch, gsn_charm_person);
    }

    if (can_see (ch->master, ch) && ch->in_room != NULL)
    {
        act ("$n stops following you.", ch, NULL, ch->master, TO_VICT);
        act ("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
    }
    if (ch->master->pet == ch)
        ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets (CHAR_DATA * ch)
{
    CHAR_DATA *pet;

    if ((pet = ch->pet) != NULL)
    {
        stop_follower (pet);
        if (pet->in_room != NULL)
            act ("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT);
        extract_char (pet, TRUE);
    }
    ch->pet = NULL;

    return;
}



void die_follower (CHAR_DATA * ch)
{
    CHAR_DATA *fch;

    if (ch->master != NULL)
    {
        if (ch->master->pet == ch)
            ch->master->pet = NULL;
        stop_follower (ch);
    }

    ch->leader = NULL;

    for (fch = char_list; fch != NULL; fch = fch->next)
    {
        if (fch->master == ch)
            stop_follower (fch);
        if (fch->leader == ch)
            fch->leader = fch;
    }

    return;
}



void do_order (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument (argument, arg);
    one_argument (argument, arg2);

    if (!str_cmp (arg2, "delete") || !str_cmp (arg2, "mob"))
    {
        send_to_char ("That will NOT be done.\n\r", ch);
        return;
    }

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        send_to_char ("Order whom to do what?\n\r", ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM))
    {
        send_to_char ("You feel like taking, not giving, orders.\n\r", ch);
        return;
    }

    if (!str_cmp (arg, "all"))
    {
        fAll = TRUE;
        victim = NULL;
    }
    else
    {
        fAll = FALSE;
        if ((victim = get_char_room (ch, arg)) == NULL)
        {
            send_to_char ("They aren't here.\n\r", ch);
            return;
        }

        if (victim == ch)
        {
            send_to_char ("Aye aye, right away!\n\r", ch);
            return;
        }

        if (!IS_AFFECTED (victim, AFF_CHARM) || victim->master != ch
            || (IS_IMMORTAL (victim) && victim->trust >= ch->trust))
        {
            send_to_char ("Do it yourself!\n\r", ch);
            return;
        }
    }

    found = FALSE;
    for (och = ch->in_room->people; och != NULL; och = och_next)
    {
        och_next = och->next_in_room;

        if (IS_AFFECTED (och, AFF_CHARM)
            && och->master == ch && (fAll || och == victim))
        {
            found = TRUE;
            sprintf (buf, "$n orders you to '%s'.", argument);
            act (buf, ch, NULL, och, TO_VICT);
            interpret (och, argument);
        }
    }

    if (found)
    {
        WAIT_STATE (ch, PULSE_VIOLENCE);
        send_to_char ("Ok.\n\r", ch);
    }
    else
        send_to_char ("You have no followers here.\n\r", ch);
    return;
}



void do_group (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument (argument, arg);

    if (arg[0] == '\0')
    {
        CHAR_DATA *gch;
        CHAR_DATA *leader;

        leader = (ch->leader != NULL) ? ch->leader : ch;
        sprintf (buf, "%s's group:\n\r", PERS (leader, ch));
        send_to_char (buf, ch);

        for (gch = char_list; gch != NULL; gch = gch->next)
        {
            if (is_same_group (gch, ch))
            {
                sprintf (buf,
                         "[%2d %s] %-16s %4d/%4d hp %4d/%4d mana %4d/%4d mv %5d xp\n\r",
                         gch->level,
                         IS_NPC (gch) ? "Mob" : class_table[gch->
                                                            class]->who_name,
                         capitalize (PERS (gch, ch)), gch->hit, gch->max_hit,
                         gch->mana, gch->max_mana, gch->move, gch->max_move,
                         gch->exp);
                send_to_char (buf, ch);
            }
        }
        return;
    }

    if ((victim = get_char_room (ch, arg)) == NULL)
    {
        send_to_char ("They aren't here.\n\r", ch);
        return;
    }

    if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch))
    {
        send_to_char ("But you are following someone else!\n\r", ch);
        return;
    }

    if (victim->master != ch && ch != victim)
    {
        act_new ("$N isn't following you.", ch, NULL, victim, TO_CHAR,
                 POS_SLEEPING);
        return;
    }

    if (IS_AFFECTED (victim, AFF_CHARM))
    {
        send_to_char ("You can't remove charmed mobs from your group.\n\r",
                      ch);
        return;
    }

    if (IS_AFFECTED (ch, AFF_CHARM))
    {
        act_new ("You like your master too much to leave $m!",
                 ch, NULL, victim, TO_VICT, POS_SLEEPING);
        return;
    }

    if (is_same_group (victim, ch) && ch != victim)
    {
        victim->leader = NULL;
        act_new ("$n removes $N from $s group.",
                 ch, NULL, victim, TO_NOTVICT, POS_RESTING);
        act_new ("$n removes you from $s group.",
                 ch, NULL, victim, TO_VICT, POS_SLEEPING);
        act_new ("You remove $N from your group.",
                 ch, NULL, victim, TO_CHAR, POS_SLEEPING);
        return;
    }

    victim->leader = ch;
    act_new ("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT,
             POS_RESTING);
    act_new ("You join $n's group.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
    act_new ("$N joins your group.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
    return;
}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0, amount_silver = 0;
    int share_gold, share_silver;
    int extra_gold, extra_silver;

    argument = one_argument (argument, arg1);
    one_argument (argument, arg2);

    if (arg1[0] == '\0')
    {
        send_to_char ("Split how much?\n\r", ch);
        return;
    }

    amount_silver = atoi (arg1);

    if (arg2[0] != '\0')
        amount_gold = atoi (arg2);

    if (amount_gold < 0 || amount_silver < 0)
    {
        send_to_char ("Your group wouldn't like that.\n\r", ch);
        return;
    }

    if (amount_gold == 0 && amount_silver == 0)
    {
        send_to_char ("You hand out zero coins, but no one notices.\n\r", ch);
        return;
    }

    if (ch->gold < amount_gold || ch->silver < amount_silver)
    {
        send_to_char ("You don't have that much to split.\n\r", ch);
        return;
    }

    members = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        if (is_same_group (gch, ch) && !IS_AFFECTED (gch, AFF_CHARM))
            members++;
    }

    if (members < 2)
    {
        send_to_char ("Just keep it all.\n\r", ch);
        return;
    }

    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    share_gold = amount_gold / members;
    extra_gold = amount_gold % members;

    if (share_gold == 0 && share_silver == 0)
    {
        send_to_char ("Don't even bother, cheapskate.\n\r", ch);
        return;
    }

    ch->silver -= amount_silver;
    ch->silver += share_silver + extra_silver;
    ch->gold -= amount_gold;
    ch->gold += share_gold + extra_gold;

    if (share_silver > 0)
    {
        sprintf (buf,
                 "You split %d silver coins. Your share is %d silver.\n\r",
                 amount_silver, share_silver + extra_silver);
        send_to_char (buf, ch);
    }

    if (share_gold > 0)
    {
        sprintf (buf,
                 "You split %d gold coins. Your share is %d gold.\n\r",
                 amount_gold, share_gold + extra_gold);
        send_to_char (buf, ch);
    }

    if (share_gold == 0)
    {
        sprintf (buf, "$n splits %d silver coins. Your share is %d silver.",
                 amount_silver, share_silver);
    }
    else if (share_silver == 0)
    {
        sprintf (buf, "$n splits %d gold coins. Your share is %d gold.",
                 amount_gold, share_gold);
    }
    else
    {
        sprintf (buf,
                 "$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
                 amount_silver, amount_gold, share_silver, share_gold);
    }

    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
    {
        if (gch != ch && is_same_group (gch, ch)
            && !IS_AFFECTED (gch, AFF_CHARM))
        {
            act (buf, ch, NULL, gch, TO_VICT);
            gch->gold += share_gold;
            gch->silver += share_silver;
        }
    }

    return;
}

void do_gtell (CHAR_DATA * ch, char *argument)
{
    CHAR_DATA *gch;

    if (argument[0] == '\0')
    {
        send_to_char ("Tell your group what?\n\r", ch);
        return;
    }

    if (IS_SET (ch->comm, COMM_NOTELL))
    {
        send_to_char ("Your message didn't get through!\n\r", ch);
        return;
    }

    act("{xYou tell the group '{C$T{x'", ch, NULL, argument, TO_CHAR);
    for (gch = char_list; gch != NULL; gch = gch->next)
    {
        if (is_same_group (gch, ch))
            act_new ("{x$n tells the group '{C$t{x'",
                     ch, argument, gch, TO_VICT, POS_SLEEPING);
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group (CHAR_DATA * ach, CHAR_DATA * bch)
{
    if (ach == NULL || bch == NULL)
        return FALSE;

    if (ach->leader != NULL)
        ach = ach->leader;
    if (bch->leader != NULL)
        bch = bch->leader;
    return ach == bch;
}

/*
 * Lopes Color, Oct-94
 *
 */
void do_color (CHAR_DATA * ch, char *argument)
{

    if (IS_NPC(ch))
    {
        send_to_char ("Color is not available for mobs or while switched.\n\r", ch);
        return;
    }

    if (!IS_SET (ch->act, PLR_COLOR))
    {
        ch->desc->ansi = TRUE;
        SET_BIT (ch->act, PLR_COLOR);
        send_to_char ("{RC{Yo{Bl{Go{Cr{x is now ON, Way Cool!\n\r", ch);
    }
    else
    {
        ch->desc->ansi = FALSE;
        REMOVE_BIT (ch->act, PLR_COLOR);
        send_to_char ("Color is now OFF, <sigh>\n\r", ch);
    }
    return;

}

/*
 * Rhien - 04/11/2015
 * Clears the screen using the VT100 escape code (if the client supports it)
 */
void do_clear(CHAR_DATA * ch, char *argument)
{
    send_to_char("\033[2J", ch);
}

/*
 * Rhien - 05/21/2015
 * Reclasses the player into a new specialized class.
 */
void do_reclass(CHAR_DATA * ch, char *argument)
{
    extern int top_group;
    char reclass_list[MAX_STRING_LENGTH];

    // Players must be at least level 10 to reclass.
    if (ch->level < 10) {
        send_to_char("You must be at least level 10 to reclass.\n\r", ch);
        return;
    }

    // Immortals can't reclass.. they must set.
    if (IS_NPC(ch) || IS_IMMORTAL(ch)) {
        send_to_char("You cannot reclass.\n\r", ch);
        return;
    }

    // Do not allow someone who is fighting or stunned to reclass.
    if (ch->position == POS_FIGHTING)
    {
        send_to_char("No way! You are fighting.\n\r", ch);
        return;
    }

    if (ch->position < POS_STUNNED)
    {
        send_to_char("You're not DEAD yet.\n\r", ch);
        return;
    }

    // Declare the things we need now that we're for sure reclassing.
    AFFECT_DATA *af, *af_next;
    int iClass = 0;
    iClass = class_lookup(argument);
    int i = 0;

    // Get the list of valid reclasses so we can show them if we need to.
    // The reclasses start at position 4, that's not changing so we'll
    // start the loop there.
    sprintf (reclass_list, "Valid reclasses are [");
    for (i = 4; i < top_class; i++)
    {
        if (class_table[i]->name == NULL)
        {
            log_string("BUG: null class");
            continue;
        }

        // Show only base classes, not reclasses.
        if (class_table[i]->is_reclass == TRUE)
        {
            if (i > 4)
                strcat (reclass_list, " ");

            strcat (reclass_list, class_table[i]->name);
        }
    }
    strcat (reclass_list, "]\n\r");

    // Check that it's a valid class and that the player can be that class
    if (iClass == -1)
    {
        send_to_char("That's not a valid class.\n\r", ch);
        send_to_char(reclass_list, ch);
        return;
    }
    else if (class_table[iClass]->is_reclass == FALSE)
    {
        send_to_char("That is a base class, you must choose a reclass.\n\r", ch);
        send_to_char(reclass_list, ch);
        return;
    }

    if (iClass == ENCHANTOR_CLASS_LOOKUP && ch->class != MAGE_CLASS_LOOKUP)
    {
        send_to_char("Only mages can reclass into enchantors.\n\r", ch);
        return;
    }

    char buf[MSL];
    int oldLevel = 0;

    sprintf(buf, "$N is reclassing to %s", class_table[iClass]->name);
    wiznet(buf, ch, NULL, WIZ_GENERAL, 0, 0);

    // This is a temporary flag to identify the player as reclassing, do NOT save this
    // flag, we want it to reset when they leave
    ch->pcdata->is_reclassing = TRUE;

    // Set the new class which is a reclass
    ch->class = iClass;

    // Level resets to 1, hit, mana and move also reset
    oldLevel = ch->level;
    ch->level = 1;

    ch->pcdata->perm_hit = 20;
    ch->max_hit = 20;
    ch->hit = 20;

    ch->pcdata->perm_mana = 100;
    ch->max_mana = 100;
    ch->mana = 100;

    ch->pcdata->perm_move = 100;
    ch->max_move = 100;
    ch->move = 100;

    // Reclassing will give a bonus so that the higher the level you were before you reclassed the more
    // initial trains/practices you get to start with.  This should encourage players to level up their
    // initial class.
    ch->train += 5;
    ch->train += oldLevel / 5;

    ch->practice += 5;
    ch->practice += oldLevel / 5;

    // Reset the users stats back to default (they will have more trains now to up them quicker).
    for (i = 0; i < MAX_STATS; i++)
    {
        ch->perm_stat[i] = pc_race_table[ch->race].stats[i];
    }

    // Get rid of any pets in case they have a high level one that we don't them to have when their level
    // is halved.
    if (ch->pet != NULL)
    {
        nuke_pets(ch);
        ch->pet = NULL;
    }

    // Remove any spells or affects so the player can't come out spelled up with
    // much higher levels spells.
    for (af = ch->affected; af != NULL; af = af_next)
    {
        af_next = af->next;
        affect_remove(ch, af);
    }

    // Call here for safety, this is also called on login.
    reset_char(ch);

    // Clear all previously known skills
    for (i = 0; i < MAX_SKILL; i++)
    {
        ch->pcdata->learned[i] = 0;
    }

    // Clear all previously known groups
    for (i = 0; i < top_group; i++)
    {
        if (group_table[i]->name == NULL)
            break;

        ch->pcdata->group_known[i] = 0;
    }

    // Add back Race skills
    for (i = 0; i < 5; i++)
    {
        if (pc_race_table[ch->race].skills[i] == NULL)
            break;

        group_add(ch, pc_race_table[ch->race].skills[i], FALSE);
    }

    // Reset points
    ch->pcdata->points = pc_race_table[ch->race].points;

    // Add back in the base groups
    group_add(ch, "rom basics", FALSE);
    group_add(ch, class_table[ch->class]->base_group, FALSE);
    ch->pcdata->learned[gsn_recall] = 50;

    sprintf(buf, "\n\r{YCongratulations{x, you are preparing to reclass as a %s.\n\r", class_table[ch->class]->name);
    send_to_char(buf, ch);

    send_to_char("\n\rDo you wish to customize this character?\n\r", ch);
    send_to_char("Customization takes time, but allows a wider range of skills and abilities.\n\r", ch);
    send_to_char("Customize (Y/N)? ", ch);

    // Move character to LIMBO so they can't be attacked or messed with while this 
    // process is happening.  If they disconnect from reclass they will end up at
    // their last save point.  Reclassing players don't save.
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));

    // Put them back in creation
    ch->desc->connected = CON_DEFAULT_CHOICE;

} // end do_reclass

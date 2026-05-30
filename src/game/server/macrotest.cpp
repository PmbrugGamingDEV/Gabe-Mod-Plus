#include "cbase.h"


/*

class CExamplePlayer
{

PUBLIC :

INT m_iHealth;
UINT m_iScore;
BOOL m_bAlive;

function Spawn()
begin
m_iHealth make 100;
m_iScore make 0;
m_bAlive make YES;

PRINT("Player spawned");
end

function AddScore(INT amount)
begin
m_iScore make m_iScore ADD amount;

PRINT("Added score");
PRINT_INT(m_iScore);
end

function TakeDamage(INT damage)
begin
IF(m_bAlive is NO)
begin
WARNING("Player already dead");
RETURN;
end

m_iHealth make m_iHealth SUB damage;

PRINT("Player damaged");
PRINT_INT(m_iHealth);

IF(m_iHealth lessthanorequalto 0)
begin
Die();
end
end

function Heal(INT amount)
begin
IF(m_bAlive is NO)
begin
WARNING("Cannot heal dead player");
RETURN;
end

m_iHealth make m_iHealth ADD amount;

IF(m_iHealth greaterthan 100)
begin
m_iHealth make 100;
end

PRINT("Player healed");
PRINT_INT(m_iHealth);
end

function Die()
begin
m_bAlive make NO;
m_iHealth make 0;

ERRORMSG("Player died");
end

end;

//=========================================================

function TestPlayer()
begin
CExamplePlayer player;

player.Spawn();

player.AddScore(500);

player.TakeDamage(25);
player.TakeDamage(90);

player.Heal(20);
end
*/
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : MonoBehaviour
{
    [SerializeField] private int enemyValue;

    public int EnemyValue
    {
        get
        {
            return enemyValue;
        }
    }
}
